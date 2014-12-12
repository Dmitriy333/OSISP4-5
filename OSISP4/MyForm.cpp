#include "MyForm.h"
#include <Windows.h>
#using <System.dll>
#using <System.Windows.Forms.dll>
#include <string>
#include <list>
#include <msclr/marshal_cppstd.h>
using namespace OSISP4;
HMODULE hMod;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew MyForm);
	return 0;
}

Void MyForm::MyForm_Load(System::Object^  sender, System::EventArgs^  e)
{
	hMod = LoadLibrary(L"PhoneBaseDll.dll");
	createBase = (CreateBase*)GetProcAddress(hMod, "createBase");
	findRecords = (FindRecords*)GetProcAddress(hMod, "find");
	createBase();
}

Void MyForm::MyForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e)
{
	closeResources = (CloseResources*)GetProcAddress(hMod, "closeResources");
	closeResources();
	FreeLibrary(hMod);
}

Void MyForm::button1_Click(System::Object^  sender, System::EventArgs^  e) {
	int i;
	MyForm::textBox2->Clear();
	System::String^ foundName = textBox1->Text;
	std::list < std::string > listRecords;
	if (!String::IsNullOrEmpty(foundName))
	{
		switch (MyForm::comboBox1->SelectedIndex){
		case 0:
			i = 0;
			break;
		case 1:
			i = 1;
			break;
		case 2:
			i = 4;
			break;
		}
		std::string str = findRecords(msclr::interop::marshal_as < std::string >(foundName), i);
		textBox2->Text += gcnew System::String(str.c_str());
	}
}
