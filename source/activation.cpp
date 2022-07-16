#include "interface_derived.hpp"
#include "activation.hpp""

void MainFrameDerived::OnActivatePersonal(wxCommandEvent& event) {
	auto dlg = new PersonalActivationDlgBase(this);
	dlg->Show();
}

void MainFrameDerived::OnActivateProPlus(wxCommandEvent& event) {
	auto dlg = new PlusProActivationDlgBase(this);
	dlg->Show();
}