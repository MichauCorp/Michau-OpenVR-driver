#include <ControllerDriver.h>
#include "SimpleSerial.h"
#include <iostream>

char com_port[] = "\\\\.\\COM6";
char name;
DWORD COM_BAUD_RATE = CBR_9600;
SimpleSerial Serial(com_port, COM_BAUD_RATE);
bool oneStr = true;
float spd;

EVRInitError ControllerDriver::Activate(uint32_t unObjectId)
{

	driverId = unObjectId; //unique ID for your driver

	PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(driverId); //this gets a container object where you store all the information about your driver

	VRProperties()->SetStringProperty(props, Prop_InputProfilePath_String, "{example}/input/controller_profile.json"); //tell OpenVR where to get your driver's Input Profile
	VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_Treadmill); //tells OpenVR what kind of device this is
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send joystick commands to OpenVR with, in the Y direction (forward/backward)
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &trackpadYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //sets up handler you'll use to send trackpad commands to OpenVR with, in the Y direction
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarType_Absolute? Take a look at the comments on EVRScalarType.
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &trackpadXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided); //Why VRScalarUnits_NormalizedTwoSided? Take a look at the comments on EVRScalarUnits.
	
	//The following properites are ones I tried out because I saw them in other samples, but I found they were not needed to get the sample working.
	//There are many samples, take a look at the openvr_header.h file. You can try them out.

	//VRProperties()->SetUint64Property(props, Prop_CurrentUniverseId_Uint64, 2);
	//VRProperties()->SetBoolProperty(props, Prop_HasControllerComponent_Bool, true);
	//VRProperties()->SetBoolProperty(props, Prop_NeverTracked_Bool, true);
	//VRProperties()->SetInt32Property(props, Prop_Axis0Type_Int32, k_eControllerAxis_TrackPad);
	//VRProperties()->SetInt32Property(props, Prop_Axis2Type_Int32, k_eControllerAxis_Joystick);
	//VRProperties()->SetStringProperty(props, Prop_SerialNumber_String, "example_controler_serial");
	//VRProperties()->SetStringProperty(props, Prop_RenderModelName_String, "vr_controller_vive_1_5");
	//uint64_t availableButtons = ButtonMaskFromId(k_EButton_SteamVR_Touchpad) |
	//	ButtonMaskFromId(k_EButton_IndexController_JoyStick);
	//VRProperties()->SetUint64Property(props, Prop_SupportedButtons_Uint64, availableButtons);

	return VRInitError_None;
}

DriverPose_t ControllerDriver::GetPose()
{
	DriverPose_t pose = { 0 }; //This example doesn't use Pose, so this method is just returning a default Pose.
	pose.poseIsValid = false;
	pose.result = TrackingResult_Calibrating_OutOfRange;
	pose.deviceIsConnected = true;

	HmdQuaternion_t quat;
	quat.w = 1;
	quat.x = 0;
	quat.y = 0;
	quat.z = 0;

	pose.qWorldFromDriverRotation = quat;
	pose.qDriverFromHeadRotation = quat;

	return pose;
}

void ControllerDriver::RunFrame()
{
	if (Serial.connected_)
	{
		string str = Serial.ReadSerialPort(7, "mine");
		VRDriverLog()->Log(str.c_str());

		try
		{
			spd = std::stof(str);
		}
		catch (const std::exception&)
		{

		}

		if (spd == 0.0)
		{
			VRDriverLog()->Log("Not moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0, 0);
		}
		else
		{
			VRDriverLog()->Log("Is Moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, spd, 0);
		}

		/*
		if (spd == 0.0)
		{
			VRDriverLog()->Log("Not moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0, 0);
		}
		else if (spd <= 0.5)
		{
			VRDriverLog()->Log("Slow Moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.65f, 0);
		}
		else
		{
			VRDriverLog()->Log("Fast Moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.95f, 0);
		}
		*/

		/*
		if (spd == 0.0)
		{
			VRDriverLog()->Log("Not moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0, 0);
		}
		else if (spd <= 0.2)
		{
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.25f, 0);
		}
		else if (spd <= 0.4)
		{
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.45f, 0);
		}
		else if (spd <= 0.6)
		{
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.65f, 0);
		}
		else
		{
			VRDriverLog()->Log("Fast Moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.95f, 0);
		}
		*/

		/*
		if (str == "0.0")
		{
			VRDriverLog()->Log("Not moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0, 0);
		}
		else
		{
			VRDriverLog()->Log("IS Moving");
			VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.65f, 0);
		}
		*/
			
	}


	//VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.95f, 0);
	//VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0, 0);
}

void ControllerDriver::Deactivate()
{
	driverId = k_unTrackedDeviceIndexInvalid;
}

void* ControllerDriver::GetComponent(const char* pchComponentNameAndVersion)
{
	//I found that if this method just returns null always, it works fine. But I'm leaving the if statement in since it doesn't hurt.
	//Check out the IVRDriverInput_Version declaration in openvr_driver.h. You can search that file for other _Version declarations 
	//to see other components that are available. You could also put a log in this class and output the value passed into this 
	//method to see what OpenVR is looking for.
	if (strcmp(IVRDriverInput_Version, pchComponentNameAndVersion) == 0)
	{
		return this;
	}
	return NULL;
}

void ControllerDriver::EnterStandby() {}

void ControllerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) 
{
	if (unResponseBufferSize >= 1)
	{
		pchResponseBuffer[0] = 0;
	}
}

SimpleSerial::SimpleSerial(char* com_port, DWORD COM_BAUD_RATE)
{
	connected_ = false;

	io_handler_ = CreateFileA(static_cast<LPCSTR>(com_port),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (io_handler_ == INVALID_HANDLE_VALUE) {

		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			printf("Warning: Handle was not attached. Reason: %s not available\n", com_port);
	}
	else {

		DCB dcbSerialParams = { 0 };

		if (!GetCommState(io_handler_, &dcbSerialParams)) {

			printf("Warning: Failed to get current serial params");
		}

		else {
			dcbSerialParams.BaudRate = COM_BAUD_RATE;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(io_handler_, &dcbSerialParams))
				printf("Warning: could not set serial port params\n");
			else {
				connected_ = true;
				PurgeComm(io_handler_, PURGE_RXCLEAR | PURGE_TXCLEAR);
			}
		}
	}
}

void SimpleSerial::CustomSyntax(string syntax_type) {
	/*
	ifstream syntaxfile_exist("syntax_config.txt");

	if (!syntaxfile_exist) {
		VRDriverLog()->Log("doesnt recognize syntax");
		ofstream syntaxfile;
		syntaxfile.open("syntax_config.txt");

		if (syntaxfile) {
			syntaxfile << "sep ,\n";
			syntaxfile.close();
		}
	}

	syntaxfile_exist.close();
	*/

	ofstream syntaxfile;
	syntaxfile.open("syntax_config.txt");

	if (syntaxfile) {
		syntaxfile << "mine { }\n";
		syntaxfile << "sep ,\n";
		syntaxfile.close();
	}

	ifstream syntaxfile_in;
	syntaxfile_in.open("syntax_config.txt");

	string line;
	bool found = false;

	if (syntaxfile_in.is_open()) {

		while (syntaxfile_in) {
			syntaxfile_in >> syntax_name_ >> front_delimiter_ >> end_delimiter_;
			getline(syntaxfile_in, line);
			//VRDriverLog()->Log(syntax_name_.c_str());
			if (syntax_name_ == syntax_type) {
				found = true;
				break;
			}
		}

		syntaxfile_in.close();

		if (!found) {
			syntax_name_ = "";
			front_delimiter_ = ' ';
			end_delimiter_ = ' ';
			printf("Warning: Could not find delimiters, may cause problems!\n");
			VRDriverLog()->Log("Warning: Could not find delimiters, may cause problems!");
		}
	}
	else
	{
		printf("Warning: No file open");
		VRDriverLog()->Log("Warning: No file open");
	}
}

string SimpleSerial::ReadSerialPort(int reply_wait_time, string syntax_type) {

	DWORD bytes_read;
	char inc_msg[1];
	string complete_inc_msg;
	bool began = false;

	CustomSyntax(syntax_type);

	unsigned long start_time = time(nullptr);

	ClearCommError(io_handler_, &errors_, &status_);

	while ((time(nullptr) - start_time) < reply_wait_time) {

		if (status_.cbInQue > 0) {

			if (ReadFile(io_handler_, inc_msg, 1, &bytes_read, NULL)) {

				if (inc_msg[0] == front_delimiter_ || began) {
					began = true;

					if (inc_msg[0] == end_delimiter_)
						return complete_inc_msg;

					if (inc_msg[0] != front_delimiter_)
						complete_inc_msg.append(inc_msg, 1);
				}
			}
			else
				return "Warning: Failed to receive data.\n";
		}
	}
	return complete_inc_msg;
}

bool SimpleSerial::WriteSerialPort(char* data_sent)
{
	DWORD bytes_sent;

	unsigned int data_sent_length = strlen(data_sent);

	if (!WriteFile(io_handler_, (void*)data_sent, data_sent_length, &bytes_sent, NULL)) {
		ClearCommError(io_handler_, &errors_, &status_);
		return false;
	}
	else
		return true;
}

bool SimpleSerial::CloseSerialPort()
{
	if (connected_) {
		connected_ = false;
		CloseHandle(io_handler_);
		return true;
	}
	else
		return false;
}

SimpleSerial::~SimpleSerial()
{
	if (connected_) {
		connected_ = false;
		CloseHandle(io_handler_);
	}
}