/*
 * Apply median cut on an image.
 *
 * tiffmedian [-c n] [-f] input output
 *     -C n		- set colortable size.  Default is 256.
 *     -f		- use Floyd-Steinberg dithering.
 *     -c lzw		- compress output with LZW
 *     -c none		- use no compression on output
 *     -c packbits	- use packbits compression on output
 *     -r n		- create output with n rows/strip of data
 * (by default the compression scheme and rows/strip are taken
 *  from the input file)
 *
 * Notes:
 *
 * [1] Floyd-Steinberg dither:
 *  I should point out that the actual fractions we used were, assuming
 *  you are at X, moving left to right:
 *
 *		    X     7/16
 *	     3/16   5/16  1/16
 *
 *  Note that the error goes to four neighbors, not three.  I think this
 *  will probably do better (at least for black and white) than the
 *  3/8-3/8-1/4 distribution, at the cost of greater processing.  I have
 *  seen the 3/8-3/8-1/4 distribution described as "our" algorithm before,
 *  but I have no idea who the credit really belongs to.

 *  Also, I should add that if you do zig-zag scanning (see my immediately
 *  previous message), it is sufficient (but not quite as good) to send
 *  half the error one pixel ahead (e.g. to the right on lines you scan
 *  left to right), and half one pixel straight down.  Again, this is for
 *  black and white;  I've not tried it with color.
 *  --
 *					    Lou Steinberg
 *
 * [2] Color Image Quantization for Frame Buffer Display, Paul Heckbert,
 *	Siggraph '82 proceedings, pp. 297-307
 */

#include "libport.h"
#include "tif_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "tiffio.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#define MAX_CMAP_SIZE 256

#define streq(a, b) (strcmp(a, b) == 0)
#define strneq(a, b, n) (strncmp(a, b, n) == 0)

#define COLOR_DEPTH 8
#define MAX_COLOR 256

#define B_DEPTH 5 /* # bits/pixel to use */
#define B_LEN (1L << B_DEPTH)

#define C_DEPTH 2
#define C_LEN (1L << C_DEPTH) /* # cells/color to use */

#define COLOR_SHIFT (COLOR_DEPTH - B_DEPTH)

typedef struct colorbox
{
    struct colorbox *next, *prev;
    int rmin, rmax;
    int gmin, gmax;
    int bmin, bmax;
    uint32_t total;
} Colorbox;

typedef struct
{
    int num_ents;
    int entries[MAX_CMAP_SIZE][2];
} C_cell;

static uint16_t rm[MAX_CMAP_SIZE], gm[MAX_CMAP_SIZE], bm[MAX_CMAP_SIZE];
static int num_colors;
static uint32_t histogram[B_LEN][B_LEN][B_LEN];
static Colorbox *freeboxes;
static Colorbox *usedboxes;
static C_cell **ColorCells;
static TIFF *in, *out;
static uint32_t rowsperstrip = (uint32_t)-1;
static uint16_t compression = (uint16_t)-1;
static uint16_t bitspersample = 1;
static uint16_t samplesperpixel;
static uint32_t imagewidth;
static uint32_t imagelength;
static uint16_t predictor = 0;

static void get_histogram(TIFF *, Colorbox *);
static void splitbox(Colorbox *);
static void shrinkbox(Colorbox *);
static void map_colortable(void);
static void quant(TIFF *, TIFF *);
static void quant_fsdither(TIFF *, TIFF *);
static Colorbox *largest_box(void);

static void usage(int);
static int processCompressOptions(char *);

#define CopyField(tag, v)                                                      \
    if (TIFFGetField(in, tag, &v))                                             \
    TIFFSetField(out, tag, v)

int main(int argc, char *argv[])
{
    int i, dither = 0;
    uint16_t shortv, config, photometric;
    Colorbox *box_list, *ptr;
    float floatv;
    uint32_t longv;
    int c;
#if !HAVE_DECL_OPTARG
    extern int optind;
    extern char *optarg;
#endif

    num_colors = MAX_CMAP_SIZE;
    while ((c = getopt(argc, argv, "c:C:r:fh")) != -1)
        switch (c)
        {
            case 'c': /* compression scheme */
                if (!processCompressOptions(optarg))
                    usage(EXIT_FAILURE);
                break;
            case 'C': /* set colormap size */
                num_colors = atoi(optarg);
                if (num_colors > MAX_CMAP_SIZE)
                {
                    fprintf(stderr, "-C: colormap too big, max %d\n",
                            MAX_CMAP_SIZE);
                    usage(EXIT_FAILURE);
                }
                if (num_colors < 2)
                {
                    fprintf(stderr, "-C: colormap too small, min %d\n", 2);
                    usage(EXIT_FAILURE);
                }
                break;
            case 'f': /* dither */
                dither = 1;
                break;
            case 'r': /* rows/strip */
                rowsperstrip = atoi(optarg);
                break;
            case 'h':
                usage(EXIT_SUCCESS);
                /*NOTREACHED*/
                break;
            case '?':
                usage(EXIT_FAILURE);
                /*NOTREACHED*/
                break;
        }
    if (argc - optind != 2)
        usage(EXIT_FAILURE);
    in = TIFFOpen(argv[optind], "r");
    if (in == NULL)
        return (EXIT_FAILURE);
    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &imagewidth);
    TIFFGetField(in, TIFFTAG_IMAGELENGTH, &imagelength);
    TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &bitspersample);
    TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
    if (bitspersample != 8 && bitspersample != 16)
    {
        fprintf(stderr, "%s: Image must have at least 8-bits/sample\n",
                argv[optind]);
        return (EXIT_FAILURE);
    }
    if (!TIFFGetField(in, TIFFTAG_PHOTOMETRIC, &photometric) ||
        photometric != PHOTOMETRIC_RGB || samplesperpixel < 3)
    {
        fprintf(stderr, "%s: Image must have RGB data\n", argv[optind]);
        return (EXIT_FAILURE);
    }
    TIFFGetField(in, TIFFTAG_PLANARCONFIG, &config);
    if (config != PLANARCONFIG_CONTIG)
    {
        fprintf(stderr, "%s: Can only handle contiguous data packing\n",
                argv[optind]);
        return (EXIT_FAILURE);
    }

    /*
     * STEP 1:  create empty boxes
     */
    usedboxes = NULL;
    box_list = freeboxes =
        (Colorbox *)_TIFFmalloc(num_colors * sizeof(Colorbox));
    freeboxes[0].next = &freeboxes[1];
    freeboxes[0].prev = NULL;
    for (i = 1; i < num_colors - 1; ++i)
    {
        freeboxes[i].next = &freeboxes[i + 1];
        freeboxes[i].prev = &freeboxes[i - 1];
    }
    freeboxes[num_colors - 1].next = NULL;
    freeboxes[num_colors - 1].prev = &freeboxes[num_colors - 2];

    /*
     * STEP 2: get histogram, initialize first box
     */
    ptr = freeboxes;
    freeboxes = ptr->next;
    if (freeboxes)
        freeboxes->prev = NULL;
    ptr->next = usedboxes;
    usedboxes = ptr;
    if (ptr->next)
        ptr->next->prev = ptr;
    get_histogram(in, ptr);

    /*
     * STEP 3: continually subdivide boxes until no more free
     * boxes remain or until all colors assigned.
     */
    while (freeboxes != NULL)
    {
        ptr = largest_box();
        if (ptr != NULL)
            splitbox(ptr);
        else
            freeboxes = NULL;
    }

    /*
     * STEP 4: assign colors to all boxes
     */
    for (i = 0, ptr = usedboxes; ptr != NULL; ++i, ptr = ptr->next)
    {
        rm[i] = ((ptr->rmin + ptr->rmax) << COLOR_SHIFT) / 2;
        gm[i] = ((ptr->gmin + ptr->gmax) << COLOR_SHIFT) / 2;
        bm[i] = ((ptr->bmin + ptr->bmax) << COLOR_SHIFT) / 2;
    }

    /* We're done with the boxes now */
    _TIFFfree(box_list);
    freeboxes = usedboxes = NULL;

    /*
     * STEP 5: scan histogram and map all values to closest color
     */
    /* 5a: create cell list as described in Heckbert[2] */
    ColorCells =
        (C_cell **)_TIFFmalloc(C_LEN * C_LEN * C_LEN * sizeof(C_cell *));
    _TIFFmemset(ColorCells, 0, C_LEN * C_LEN * C_LEN * sizeof(C_cell *));
    /* 5b: create mapping from truncated pixel space to color
       table entries */
    map_colortable();

    /*
     * STEP 6: scan image, match input values to table entries
     */
    out = TIFFOpen(argv[optind + 1], "w");
    if (out == NULL)
    {
        _TIFFfree(ColorCells);
        return (EXIT_FAILURE);
    }

    CopyField(TIFFTAG_SUBFILETYPE, longv);
    CopyField(TIFFTAG_IMAGEWIDTH, longv);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, (short)COLOR_DEPTH);
    if (compression != (uint16_t)-1)
    {
        TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
        switch (compression)
        {
            case COMPRESSION_LZW:
            case COMPRESSION_ADOBE_DEFLATE:
            case COMPRESSION_DEFLATE:
                if (predictor != 0)
                    TIFFSetField(out, TIFFTAG_PREDICTOR, predictor);
                break;
        }
    }
    else
        CopyField(TIFFTAG_COMPRESSION, compression);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, (short)PHOTOMETRIC_PALETTE);
    CopyField(TIFFTAG_ORIENTATION, shortv);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, (short)1);
    CopyField(TIFFTAG_PLANARCONFIG, shortv);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
                 TIFFDefaultStripSize(out, rowsperstrip));
    CopyField(TIFFTAG_MINSAMPLEVALUE, shortv);
    CopyField(TIFFTAG_MAXSAMPLEVALUE, shortv);
    CopyField(TIFFTAG_RESOLUTIONUNIT, shortv);
    CopyField(TIFFTAG_XRESOLUTION, floatv);
    CopyField(TIFFTAG_YRESOLUTION, floatv);
    CopyField(TIFFTAG_XPOSITION, floatv);
    CopyField(TIFFTAG_YPOSITION, floatv);

    if (dither)
        quant_fsdither(in, out);
    else
        quant(in, out);
        /*
         * Scale colormap to TIFF-required 16-bit values.
         */
#define SCALE(x) (((x) * ((1L << 16) - 1)) / 255)
    for (i = 0; i < MAX_CMAP_SIZE; ++i)
    {
        rm[i] = SCALE(rm[i]);
        gm[i] = SCALE(gm[i]);
        bm[i] = SCALE(bm[i]);
    }
    TIFFSetField(out, TIFFTAG_COLORMAP, rm, gm, bm);
    (void)TIFFClose(out);
    _TIFFfree(ColorCells);
    return (EXIT_SUCCESS);
}

static int processCompressOptions(char *opt)
{
    if (streq(opt, "none"))
        compression = COMPRESSION_NONE;
    else if (streq(opt, "packbits"))
        compression = COMPRESSION_PACKBITS;
    else if (strneq(opt, "lzw", 3))
    {
        char *cp = strchr(opt, ':');
        if (cp)
            predictor = atoi(cp + 1);
        compression = COMPRESSION_LZW;
    }
    else if (strneq(opt, "zip", 3))
    {
        char *cp = strchr(opt, ':');
        if (cp)
            predictor = atoi(cp + 1);
        compression = COMPRESSION_ADOBE_DEFLATE;
    }
    else
        return (0);
    return (1);
}

static const char usage_info[] =
    "Apply the median cut algorithm to an RGB TIFF file\n\n"
    "usage: tiffmedian [options] input.tif output.tif\n"
    "where options are:\n"
    " -r #  make each strip have no more than # rows\n"
    " -C #  create a colormap with # entries\n"
    " -f    use Floyd-Steinberg dithering\n"
    "\n"
#ifdef LZW_SUPPORT
    " -c lzw[:opts] compress output with Lempel-Ziv & Welch encoding\n"
    /* "    LZW options:" */
    "    #  set predictor value\n"
    "    For example, -c lzw:2 to get LZW-encoded data with horizontal "
    "differencing\n"
#endif
#ifdef ZIP_SUPPORT
    " -c zip[:opts] compress output with deflate encoding\n"
    /* "    Deflate (ZIP) options:" */
    "    #  set predictor value\n"
#endif
#ifdef PACKBITS_SUPPORT
    " -c packbits   compress output with packbits encoding\n"
#endif
#if defined(LZW_SUPPORT) || defined(ZIP_SUPPORT) || defined(PACKBITS_SUPPORT)
    " -c none       use no compression algorithm on output\n"
#endif
    "\n";

static void usage(int code)
{
    FILE *out = (code == EXIT_SUCCESS) ? stdout : stderr;

    fprintf(out, "%s\n\n", TIFFGetVersion());
    fprintf(out, "%s", usage_info);
    exit(code);
}

static void get_histogram(TIFF *in, Colorbox *box)
{
    register unsigned char *inptr;
    register int red, green, blue;
    register uint32_t j, i;
    unsigned char *inputline;

    inputline = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(in));
    if (inputline == NULL)
    {
        fprintf(stderr, "No space for scanline buffer\n");
        exit(EXIT_FAILURE);
    }
    box->rmin = box->gmin = box->bmin = 999;
    box->rmax = box->gmax = box->bmax = -1;
    box->total = imagewidth * imagelength;

    {
        register uint32_t *ptr = &histogram[0][0][0];
        for (i = B_LEN * B_LEN * B_LEN; i-- > 0;)
            *ptr++ = 0;
    }
    for (i = 0; i < imagelength; i++)
    {
        if (TIFFReadScanline(in, inputline, i, 0) <= 0)
            break;
        inptr = inputline;
        for (j = imagewidth; j-- > 0;)
        {
            red = (*inptr++) & 0xff >> COLOR_SHIFT;
            green = (*inptr++) & 0xff >> COLOR_SHIFT;
            blue = (*inptr++) & 0xff >> COLOR_SHIFT;
            if ((red | green | blue) >= B_LEN)
            {
                fprintf(stderr, "Logic error. "
                                "Histogram array overflow!\n");
                exit(EXIT_FAILURE);
            }
            if (red < box->rmin)
                box->rmin = red;
            if (red > box->rmax)
                box->rmax = red;
            if (green < box->gmin)
                box->gmin = green;
            if (green > box->gmax)
                box->gmax = green;
            if (blue < box->bmin)
                box->bmin = blue;
            if (blue > box->bmax)
                box->bmax = blue;
            histogram[red][green][blue]++;
        }
    }
    _TIFFfree(inputline);
}

static Colorbox *largest_box(void)
{
    register Colorbox *p, *b;
    register uint32_t size;

    b = NULL;
    size = 0;
    for (p = usedboxes; p != NULL; p = p->next)
        if ((p->rmax > p->rmin || p->gmax > p->gmin || p->bmax > p->bmin) &&
            p->total > size)
            size = (b = p)->total;
    return (b);
}

static void splitbox(Colorbox *ptr)
{
    uint32_t hist2[B_LEN];
    int first = 0, last = 0;
    register Colorbox *new;
    register uint32_t *iptr, *histp;
    register int i, j;
    register int ir, ig, ib;
    register uint32_t sum, sum1, sum2;
    enum
    {
        RED,
        GREEN,
        BLUE
    } axis;

    /*
     * See which axis is the largest, do a histogram along that
     * axis.  Split at median point.  Contract both new boxes to
     * fit points and return
     */
    i = ptr->rmax - ptr->rmin;
    if (i >= ptr->gmax - ptr->gmin && i >= ptr->bmax - ptr->bmin)
        axis = RED;
    else if (ptr->gmax - ptr->gmin >= ptr->bmax - ptr->bmin)
        axis = GREEN;
    else
        axis = BLUE;
    /* get histogram along longest axis */
    switch (axis)
    {
        case RED:
            histp = &hist2[ptr->rmin];
            for (ir = ptr->rmin; ir <= ptr->rmax; ++ir)
            {
                *histp = 0;
                for (ig = ptr->gmin; ig <= ptr->gmax; ++ig)
                {
                    iptr = &histogram[ir][ig][ptr->bmin];
                    for (ib = ptr->bmin; ib <= ptr->bmax; ++ib)
                        *histp += *iptr++;
                }
                histp++;
            }
            first = ptr->rmin;
            last = ptr->rmax;
            break;
        case GREEN:
            histp = &hist2[ptr->gmin];
            for (ig = ptr->gmin; ig <= ptr->gmax; ++ig)
            {
                *histp = 0;
                for (ir = ptr->rmin; ir <= ptr->rmax; ++ir)
                {
                    iptr = &histogram[ir][ig][ptr->bmin];
                    for (ib = ptr->bmin; ib <= ptr->bmax; ++ib)
                        *histp += *iptr++;
                }
                histp++;
            }
            first = ptr->gmin;
            last = ptr->gmax;
            break;
        case BLUE:
            histp = &hist2[ptr->bmin];
            for (ib = ptr->bmin; ib <= ptr->bmax; ++ib)
            {
                *histp = 0;
                for (ir = ptr->rmin; ir <= ptr->rmax; ++ir)
                {
                    iptr = &histogram[ir][ptr->gmin][ib];
                    for (ig = ptr->gmin; ig <= ptr->gmax; ++ig)
                    {
                        *histp += *iptr;
                        iptr += B_LEN;
                    }
                }
                histp++;
            }
            first = ptr->bmin;
            last = ptr->bmax;
            break;
    }
    /* find median point */
    sum2 = ptr->total / 2;
    histp = &hist2[first];
    sum = 0;
    for (i = first; i <= last && (sum += *histp++) < sum2; ++i)
        ;
    if (i == first)
        i++;

    /* Create new box, re-allocate points */
    new = freeboxes;
    freeboxes = new->next;
    if (freeboxes)
        freeboxes->prev = NULL;
    if (usedboxes)
        usedboxes->prev = new;
    new->next = usedboxes;
    usedboxes = new;

    histp = &hist2[first];
    for (sum1 = 0, j = first; j < i; j++)
        sum1 += *histp++;
    for (sum2 = 0, j = i; j <= last; j++)
        sum2 += *histp++;
    new->total = sum1;
    ptr->total = sum2;

    new->rmin = ptr->rmin;
    new->rmax = ptr->rmax;
    new->gmin = ptr->gmin;
    new->gmax = ptr->gmax;
    new->bmin = ptr->bmin;
    new->bmax = ptr->bmax;
    switch (axis)
    {
        case RED:
            new->rmax = i - 1;
            ptr->rmin = i;
            break;
        case GREEN:
            new->gmax = i - 1;
            ptr->gmin = i;
            break;
        case BLUE:
            new->bmax = i - 1;
            ptr->bmin = i;
            break;
    }
    shrinkbox(new);
    shrinkbox(ptr);
}

static void shrinkbox(Colorbox *box)
{
    register uint32_t *histp;
    register int ir, ig, ib;

    if (box->rmax > box->rmin)
    {
        for (ir = box->rmin; ir <= box->rmax; ++ir)
            for (ig = box->gmin; ig <= box->gmax; ++ig)
            {
                histp = &histogram[ir][ig][box->bmin];
                for (ib = box->bmin; ib <= box->bmax; ++ib)
                    if (*histp++ != 0)
                    {
                        box->rmin = ir;
                        goto have_rmin;
                    }
            }
    have_rmin:
        if (box->rmax > box->rmin)
            for (ir = box->rmax; ir >= box->rmin; --ir)
                for (ig = box->gmin; ig <= box->gmax; ++ig)
                {
                    histp = &histogram[ir][ig][box->bmin];
                    ib = box->bmin;
                    for (; ib <= box->bmax; ++ib)
                        if (*histp++ != 0)
                        {
                            box->rmax = ir;
                            goto have_rmax;
                        }
                }
    }
have_rmax:
    if (box->gmax > box->gmin)
    {
        for (ig = box->gmin; ig <= box->gmax; ++ig)
            for (ir = box->rmin; ir <= box->rmax; ++ir)
            {
                histp = &histogram[ir][ig][box->bmin];
                for (ib = box->bmin; ib <= box->bmax; ++ib)
                    if (*histp++ != 0)
                    {
                        box->gmin = ig;
                        goto have_gmin;
                    }
            }
    have_gmin:
        if (box->gmax > box->gmin)
            for (ig = box->gmax; ig >= box->gmin; --ig)
                for (ir = box->rmin; ir <= box->rmax; ++ir)
                {
                    histp = &histogram[ir][ig][box->bmin];
                    ib = box->bmin;
                    for (; ib <= box->bmax; ++ib)
                        if (*histp++ != 0)
                        {
                            box->gmax = ig;
                            goto have_gmax;
                        }
                }
    }
have_gmax:
    if (box->bmax > box->bmin)
    {
        for (ib = box->bmin; ib <= box->bmax; ++ib)
            for (ir = box->rmin; ir <= box->rmax; ++ir)
            {
                histp = &histogram[ir][box->gmin][ib];
                for (ig = box->gmin; ig <= box->gmax; ++ig)
                {
                    if (*histp != 0)
                    {
                        box->bmin = ib;
                        goto have_bmin;
                    }
                    histp += B_LEN;
                }
            }
    have_bmin:
        if (box->bmax > box->bmin)
            for (ib = box->bmax; ib >= box->bmin; --ib)
                for (ir = box->rmin; ir <= box->rmax; ++ir)
                {
                    histp = &histogram[ir][box->gmin][ib];
                    ig = box->gmin;
                    for (; ig <= box->gmax; ++ig)
                    {
                        if (*histp != 0)
                        {
                            box->bmax = ib;
                            goto have_bmax;
                        }
                        histp += B_LEN;
                    }
                }
    }
have_bmax:;
}

static C_cell *create_colorcell(int red, int green, int blue)
{
    register int ir, ig, ib, i;
    register C_cell *ptr;
    int mindist, next_n;
    register int tmp, dist, n;

    ir = red >> (COLOR_DEPTH - C_DEPTH);
    ig = green >> (COLOR_DEPTH - C_DEPTH);
    ib = blue >> (COLOR_DEPTH - C_DEPTH);
    ptr = (C_cell *)_TIFFmalloc(sizeof(C_cell));
    *(ColorCells + ir * C_LEN * C_LEN + ig * C_LEN + ib) = ptr;
    ptr->num_ents = 0;

    /*
     * Step 1: find all colors inside this cell, while we're at
     *	   it, find distance of centermost point to furthest corner
     */
    mindist = 99999999;
    for (i = 0; i < num_colors; ++i)
    {
        if (rm[i] >> (COLOR_DEPTH - C_DEPTH) != ir ||
            gm[i] >> (COLOR_DEPTH - C_DEPTH) != ig ||
            bm[i] >> (COLOR_DEPTH - C_DEPTH) != ib)
            continue;
        ptr->entries[ptr->num_ents][0] = i;
        ptr->entries[ptr->num_ents][1] = 0;
        ++ptr->num_ents;
        tmp = rm[i] - red;
        if (tmp < (MAX_COLOR / C_LEN / 2))
            tmp = MAX_COLOR / C_LEN - 1 - tmp;
        dist = tmp * tmp;
        tmp = gm[i] - green;
        if (tmp < (MAX_COLOR / C_LEN / 2))
            tmp = MAX_COLOR / C_LEN - 1 - tmp;
        dist += tmp * tmp;
        tmp = bm[i] - blue;
        if (tmp < (MAX_COLOR / C_LEN / 2))
            tmp = MAX_COLOR / C_LEN - 1 - tmp;
        dist += tmp * tmp;
        if (dist < mindist)
            mindist = dist;
    }

    /*
     * Step 3: find all points within that distance to cell.
     */
    for (i = 0; i < num_colors; ++i)
    {
        if (rm[i] >> (COLOR_DEPTH - C_DEPTH) == ir &&
            gm[i] >> (COLOR_DEPTH - C_DEPTH) == ig &&
            bm[i] >> (COLOR_DEPTH - C_DEPTH) == ib)
            continue;
        dist = 0;
        if ((tmp = red - rm[i]) > 0 ||
            (tmp = rm[i] - (red + MAX_COLOR / C_LEN - 1)) > 0)
            dist += tmp * tmp;
        if ((tmp = green - gm[i]) > 0 ||
            (tmp = gm[i] - (green + MAX_COLOR / C_LEN - 1)) > 0)
            dist += tmp * tmp;
        if ((tmp = blue - bm[i]) > 0 ||
            (tmp = bm[i] - (blue + MAX_COLOR / C_LEN - 1)) > 0)
            dist += tmp * tmp;
        if (dist < mindist)
        {
            ptr->entries[ptr->num_ents][0] = i;
            ptr->entries[ptr->num_ents][1] = dist;
            ++ptr->num_ents;
        }
    }

    /*
     * Sort color cells by distance, use cheap exchange sort
     */
    for (n = ptr->num_ents - 1; n > 0; n = next_n)
    {
        next_n = 0;
        for (i = 0; i < n; ++i)
            if (ptr->entries[i][1] > ptr->entries[i + 1][1])
            {
                tmp = ptr->entries[i][0];
                ptr->entries[i][0] = ptr->entries[i + 1][0];
                ptr->entries[i + 1][0] = tmp;
                tmp = ptr->entries[i][1];
                ptr->entries[i][1] = ptr->entries[i + 1][1];
                ptr->entries[i + 1][1] = tmp;
                next_n = i;
            }
    }
    return (ptr);
}

static void map_colortable(void)
{
    register uint32_t *histp = &histogram[0][0][0];
    register C_cell *cell = NULL;
    register int j, tmp, d2, dist;
    int ir, ig, ib, i;

    for (ir = 0; ir < B_LEN; ++ir)
        for (ig = 0; ig < B_LEN; ++ig)
            for (ib = 0; ib < B_LEN; ++ib, histp++)
            {
                if (*histp == 0)
                {
                    *histp = -1;
                    continue;
                }
                cell = *(ColorCells +
                         (((ir >> (B_DEPTH - C_DEPTH)) << C_DEPTH * 2) +
                          ((ig >> (B_DEPTH - C_DEPTH)) << C_DEPTH) +
                          (ib >> (B_DEPTH - C_DEPTH))));
                if (cell == NULL)
                    cell =
                        create_colorcell(ir << COLOR_SHIFT, ig << COLOR_SHIFT,
                                         ib << COLOR_SHIFT);
                dist = 9999999;
                for (i = 0; i < cell->num_ents && dist > cell->entries[i][1];
                     ++i)
                {
                    j = cell->entries[i][0];
                    d2 = rm[j] - (ir << COLOR_SHIFT);
                    d2 *= d2;
                    tmp = gm[j] - (ig << COLOR_SHIFT);
                    d2 += tmp * tmp;
                    tmp = bm[j] - (ib << COLOR_SHIFT);
                    d2 += tmp * tmp;
                    if (d2 < dist)
                    {
                        dist = d2;
                        *histp = j;
                    }
                }
            }
    _TIFFfree(cell);
}

/*
 * straight quantization.  Each pixel is mapped to the colors
 * closest to it.  Color values are rounded to the nearest color
 * table entry.
 */
static void quant(TIFF *in, TIFF *out)
{
    unsigned char *outline, *inputline;
    register unsigned char *outptr, *inptr;
    register uint32_t i, j;
    register int red, green, blue;

    inputline = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(in));
    outline = (unsigned char *)_TIFFmalloc(imagewidth);
    for (i = 0; i < imagelength; i++)
    {
        if (TIFFReadScanline(in, inputline, i, 0) <= 0)
            break;
        inptr = inputline;
        outptr = outline;
        for (j = 0; j < imagewidth; j++)
        {
            red = *inptr++ >> COLOR_SHIFT;
            green = *inptr++ >> COLOR_SHIFT;
            blue = *inptr++ >> COLOR_SHIFT;
            *outptr++ = (unsigned char)histogram[red][green][blue];
        }
        if (TIFFWriteScanline(out, outline, i, 0) < 0)
            break;
    }
    _TIFFfree(inputline);
    _TIFFfree(outline);
}

#define SWAP(type, a, b)                                                       \
    {                                                                          \
        type p;                                                                \
        p = a;                                                                 \
        a = b;                                                                 \
        b = p;                                                                 \
    }

#define GetInputLine(tif, row, bad)                                            \
    do                                                                         \
    {                                                                          \
        if (TIFFReadScanline(tif, inputline, row, 0) <= 0)                     \
            bad;                                                               \
        inptr = inputline;                                                     \
        nextptr = nextline;                                                    \
        for (j = 0; j < imagewidth; ++j)                                       \
        {                                                                      \
            *nextptr++ = *inptr++;                                             \
            *nextptr++ = *inptr++;                                             \
            *nextptr++ = *inptr++;                                             \
        }                                                                      \
    } while (0);
#define GetComponent(raw, cshift, c)                                           \
    do                                                                         \
    {                                                                          \
        cshift = raw;                                                          \
        if (cshift < 0)                                                        \
            cshift = 0;                                                        \
        else if (cshift >= MAX_COLOR)                                          \
            cshift = MAX_COLOR - 1;                                            \
        c = cshift;                                                            \
        cshift >>= COLOR_SHIFT;                                                \
    } while (0);

static void quant_fsdither(TIFF *in, TIFF *out)
{
    unsigned char *outline, *inputline, *inptr;
    short *thisline, *nextline;
    register unsigned char *outptr;
    register short *thisptr, *nextptr;
    register uint32_t i, j;
    uint32_t imax, jmax;
    int lastline, lastpixel;

    imax = imagelength - 1;
    jmax = imagewidth - 1;
    inputline = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(in));
    thisline = (short *)_TIFFmalloc(imagewidth * 3 * sizeof(short));
    nextline = (short *)_TIFFmalloc(imagewidth * 3 * sizeof(short));
    outline = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(out));

    GetInputLine(in, 0, goto bad); /* get first line */
    for (i = 1; i <= imagelength; ++i)
    {
        SWAP(short *, thisline, nextline);
        lastline = (i >= imax);
        if (i <= imax)
            GetInputLine(in, i, break);
        thisptr = thisline;
        nextptr = nextline;
        outptr = outline;
        for (j = 0; j < imagewidth; ++j)
        {
            int red, green, blue;
            register int oval, r2, g2, b2;

            lastpixel = (j == jmax);
            GetComponent(*thisptr++, r2, red);
            GetComponent(*thisptr++, g2, green);
            GetComponent(*thisptr++, b2, blue);
            oval = histogram[r2][g2][b2];
            if (oval == -1)
            {
                int ci;
                register int cj, tmp, d2, dist;
                register C_cell *cell;

                cell = *(ColorCells +
                         (((r2 >> (B_DEPTH - C_DEPTH)) << C_DEPTH * 2) +
                          ((g2 >> (B_DEPTH - C_DEPTH)) << C_DEPTH) +
                          (b2 >> (B_DEPTH - C_DEPTH))));
                if (cell == NULL)
                    cell = create_colorcell(red, green, blue);
                dist = 9999999;
                for (ci = 0; ci < cell->num_ents && dist > cell->entries[ci][1];
                     ++ci)
                {
                    cj = cell->entries[ci][0];
                    d2 = (rm[cj] >> COLOR_SHIFT) - r2;
                    d2 *= d2;
                    tmp = (gm[cj] >> COLOR_SHIFT) - g2;
                    d2 += tmp * tmp;
                    tmp = (bm[cj] >> COLOR_SHIFT) - b2;
                    d2 += tmp * tmp;
                    if (d2 < dist)
                    {
                        dist = d2;
                        oval = cj;
                    }
                }
                histogram[r2][g2][b2] = oval;
            }
            *outptr++ = oval;
            red -= rm[oval];
            green -= gm[oval];
            blue -= bm[oval];
            if (!lastpixel)
            {
                thisptr[0] += blue * 7 / 16;
                thisptr[1] += green * 7 / 16;
                thisptr[2] += red * 7 / 16;
            }
            if (!lastline)
            {
                if (j != 0)
                {
                    nextptr[-3] += blue * 3 / 16;
                    nextptr[-2] += green * 3 / 16;
                    nextptr[-1] += red * 3 / 16;
                }
                nextptr[0] += blue * 5 / 16;
                nextptr[1] += green * 5 / 16;
                nextptr[2] += red * 5 / 16;
                if (!lastpixel)
                {
                    nextptr[3] += blue / 16;
                    nextptr[4] += green / 16;
                    nextptr[5] += red / 16;
                }
                nextptr += 3;
            }
        }
        if (TIFFWriteScanline(out, outline, i - 1, 0) < 0)
            break;
    }
bad:
    _TIFFfree(inputline);
    _TIFFfree(thisline);
    _TIFFfree(nextline);
    _TIFFfree(outline);
}
