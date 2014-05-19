

function varargout = mjukvaru_gui(varargin)
% MJUKVARU_GUI MATLAB code for mjukvaru_gui.fig
%      MJUKVARU_GUI, by itself, creates a new MJUKVARU_GUI or raises the existing
%      singleton*.
%
%      H = MJUKVARU_GUI returns the handle to a new MJUKVARU_GUI or the handle to
%      the existing singleton*.
%
%      MJUKVARU_GUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in MJUKVARU_GUI.M with the given input arguments.
%
%      MJUKVARU_GUI('Property','Value',...) creates a new MJUKVARU_GUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before mjukvaru_gui_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to mjukvaru_gui_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help mjukvaru_gui

% Last Modified by GUIDE v2.5 19-May-2014 19:59:17

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @mjukvaru_gui_OpeningFcn, ...
                   'gui_OutputFcn',  @mjukvaru_gui_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before mjukvaru_gui is made visible.
function mjukvaru_gui_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to mjukvaru_gui (see VARARGIN)

% Choose default command line output for mjukvaru_gui
handles.output = hObject;

handles.sensor_and_map_plot = figure();
%Update handles structure
guidata(hObject, handles);

% UIWAIT makes mjukvaru_gui wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = mjukvaru_gui_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes during object creation, after setting all properties.
function figure1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

guidata(hObject, handles); % Typ uppdaterar hObject med handles

% --- Executes on key press with focus on figure1 and none of its controls.
function figure1_KeyPressFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  structure with the following fields (see FIGURE)
%	Key: name of the key that was pressed, in lower case
%	Character: character interpretation of the key(s) that was pressed
%	Modifier: name(s) of the modifier key(s) (i.e., control, shift) pressed
% handles    structure with handles and user data (see GUIDATA)
set(handles.styrform, 'String', 'Manuell')

switch eventdata.Key
    case 'w'
        set(handles.kommando,'String','Fram') 
        %Kommandot för forward är 0x11
        fwrite(handles.BT, uint8(17));       
    case 's'
        set(handles.kommando,'String','Back')
        %Kommandot för backward är 0x12
        fwrite(handles.BT, uint8(18));  
    case 'a'
        set(handles.kommando,'String','Rotera vänster') 
        %Kommandot för rotate_left är 0x16
        fwrite(handles.BT, uint8(22));       
    case 'd'
        set(handles.kommando,'String','Rotera höger')
        %Kommandot för rotate_right är 0x15
        fwrite(handles.BT, uint8(21));
    case 'q'
        set(handles.kommando,'String','Sväng vänster')
        %Kommandot för turn_left är 0x14
        fwrite(handles.BT, uint8(20));
    case 'e'
        set(handles.kommando,'String','Sväng höger')
        %Kommandot för turn_right är 0x13
        fwrite(handles.BT, uint8(19)); 
    case 'space'
        set(handles.kommando,'String','Stanna')
        %Kommandot för stop är 0x10
        fwrite(handles.BT, uint8(16)); 
    case 'x'
        set(handles.kommando,'String','greppa med gripklo')
        %Kommandot för open_claw är 0x18
        fwrite(handles.BT, uint8(24)); 
    case 'z'
        set(handles.kommando,'String','öppna gripklo')
        %Kommandot för close_claw är 0x17
        fwrite(handles.BT, uint8(23)); 
        
end


% --- Executes on key release with focus on figure1 and none of its controls.
function figure1_KeyReleaseFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  structure with the following fields (see FIGURE)
%	Key: name of the key that was released, in lower case
%	Character: character interpretation of the key(s) that was released
%	Modifier: name(s) of the modifier key(s) (i.e., control, shift) released
% handles    structure with handles and user data (see GUIDATA)
set(handles.kommando,'String','Stop')
fwrite(handles.BT, uint8(0));

   

% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%info = instrhwinfo('Bluetooth');
%info.RemoteNames
%Lassie_info = instrhwinfo('Bluetooth', '00066602D47F')
%Device ID: 00066602D47F
global sensor_data;
sensor_data = zeros(5,70);
global map_links;
map_links = [];
set(handles.bt_info, 'String', 'Wait...');
BT = Bluetooth('FireFly-D47F', 1); % Channel är möjligtvis inte alltid 1
disp('Blåtansobjekt skapat.')

BT.BytesAvailableFcnCount = 1;
BT.BytesAvailableFcnMode = 'byte';
BT.BytesAvailableFcn = {'mycallback', BT, handles};

fopen(BT);
disp('Kommunikationskanal öppnad.')
set(handles.bt_info, 'String', get(BT, 'status'));
set(handles.pushbutton3,'Enable','on')
set(handles.pushbutton2,'Enable','off')
handles.BT = BT;
guidata(hObject, handles);


% --- Executes on button press in pushbutton3.
function pushbutton3_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
set(handles.bt_info, 'String', 'Wait...');
fclose(handles.BT);
%clear(handles.BT); % ger error just nu
disp('Koppel frånkopplad')
set(handles.pushbutton3,'Enable','off')
set(handles.pushbutton2,'Enable','on')
set(handles.bt_info, 'String', 'Disonnected');
