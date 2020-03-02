# This script must be run from make! Use `make linux-pkg` 

# download latest AppImageTool if not downloaded
if [ ! -f "appimagetool.AppImage" ]; then 
	wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-`uname --m`.AppImage -O appimagetool.AppImage
	chmod +x appimagetool.AppImage
fi

appimagepath=AppDir

mkdir -p $appimagepath

# copy necessary pieces
cp $build_dir/$name $appimagepath	# executable
cp $src/wxlin.xpm $appimagepath		# icon
# copy libraries
cp $lib_dir/libwx_baseu-3.1.so $appimagepath
cp $lib_dir/libwx_baseu-3.1.so.3 $appimagepath
cp $lib_dir/libwx_baseu-3.1.so.3.0.0 $appimagepath
cp $lib_dir/libwx_baseu_net-3.1.so $appimagepath
cp $lib_dir/libwx_baseu_net-3.1.so.3 $appimagepath
cp $lib_dir/libwx_baseu_net-3.1.so.3.0.0 $appimagepath
cp $lib_dir/libwx_baseu_xml-3.1.so $appimagepath
cp $lib_dir/libwx_baseu_xml-3.1.so.3 $appimagepath
cp $lib_dir/libwx_baseu_xml-3.1.so.3.0.0 $appimagepath
cp $lib_dir/libwx_gtk3u_core-3.1.so $appimagepath
cp $lib_dir/libwx_gtk3u_core-3.1.so.3 $appimagepath
cp $lib_dir/libwx_gtk3u_core-3.1.so.3.0.0 $appimagepath
cp $lib_dir/libwx_gtk3u_html-3.1.so $appimagepath
cp $lib_dir/libwx_gtk3u_html-3.1.so.3 $appimagepath
cp $lib_dir/libwx_gtk3u_html-3.1.so.3.0.0 $appimagepath
cp $lib_dir/libwx_gtk3u_qa-3.1.so $appimagepath
cp $lib_dir/libwx_gtk3u_qa-3.1.so.3 $appimagepath
cp $lib_dir/libwx_gtk3u_qa-3.1.so.3.0.0 $appimagepath
cp $lib_dir/libwx_gtk3u_xrc-3.1.so $appimagepath
cp $lib_dir/libwx_gtk3u_xrc-3.1.so.3 $appimagepath
cp $lib_dir/libwx_gtk3u_xrc-3.1.so.3.0.0 $appimagepath


# generate AppImage pieces
printf "#!/bin/sh\ncd \"\$(dirname \"\$0\")\"; echo 'starting!'; LD_LIBRARY_PATH=. ./$name" > $appimagepath/AppRun
chmod +x $appimagepath/AppRun
printf "[Desktop Entry]\nType=Application\nName=$name\nIcon=wxlin\nCategories=X-None;" > $appimagepath/$name.desktop

# run AppImageTool
ARCH=`uname --m` ./appimagetool.AppImage $appimagepath

#copy to linux build folder

#cleanup
rm -r $appimagepath
