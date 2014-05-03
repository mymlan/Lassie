

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

% Last Modified by GUIDE v2.5 03-May-2014 12:10:30

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

% Update handles structure
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
load('mjukvaruvariabler') % Laddar variabler
handles.IR1_tid = IR1_tid; % L�gger till variabel i handles
handles.IR2_tid = IR2_tid;
handles.IR3_tid = IR3_tid;
handles.IR4_tid = IR4_tid;
handles.IR5_tid = IR5_tid;
handles.coord1 = coord1;
handles.coord2 = coord2;

% �vriga variabler fr�n mjukvara.m skrivs h�r
guidata(hObject, handles); % Typ uppdaterar hObject med handles


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
axes(handles.axes1)
title('IR-v�rden �ver tid')
hold on
set(handles.ir_f,'String',20.7)
set(handles.ir_v_f,'String',3.1)
set(handles.ir_h_f,'String',5.2)
set(handles.ir_v_b,'String',8.0)
set(handles.ir_h_b,'String',4.4)

%Just nu: N�r vi trycker p� knappen kommer nya sensorv�rden (i dagsl�get 
%dummyv�rden)att h�mtas. fread l�ser fr�n buffert fr�n bl�tand. handles. BT
%anger att det �r bl�tandsobjektet BT som vi l�ser fr�n. 8 �r antalet bytes
%vi max kommer vilja l�sa (1 id-byte och 7 sensorv�rden). Det vi f�r ut �r 
%en vektor Sensor_data med de 8 bytesen. Count anger antal l�st data och
%msg meddelar om l�sning misslyckats.

%[Sensor_data, count, msg] = fread(handles.BT, 8); 

%Det vi vill g�ra nu �r att l�sa av den f�rsta byten i vektorn som �r
%id_byten. Beroende p� vad vi har f�r id_byte vill vi l�gga in v�rdena i
%olika vektorer. En vektor f�r varje sensor. Varje g�ng vi h�mtar nya
%sensorv�rden vill vi l�gga in de nya v�rdena i r�tt vektor och plotta den
%nya vektorn. ex ir_h�ger_fram = [ir_h�ger_fram, Sensor_data]

%hur vet vi n�r det finns ny data att l�sa. Finns det flaggor motsvarande
%RXC1, TXC1 och UDRE1? 

%H�r plottas v�rdena upp
plot([1,2],'yellow')
plot(handles.IR2_tid,'red')
plot(handles.IR3_tid,'green')
plot(handles.IR4_tid,'blue')
plot(handles.IR5_tid,'black')

axes(handles.axes2)
title('Representation av labyrint')
hold on

min_c1 = min(handles.coord1);
min_c2 = min(handles.coord2);
max_c1 = max(handles.coord1);
max_c2 = max(handles.coord2);
axis([min_c1(1,1)-1 max_c1(1,1)+1 min_c2(1,1)-1 max_c2(1,1)+1]);

line(handles.coord1, handles.coord2)
robot_coord = [handles.coord1(end) handles.coord2(end)];
plot(robot_coord, 'o')


% --- Executes on key press with focus on figure1 and none of its controls.
function figure1_KeyPressFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  structure with the following fields (see FIGURE)
%	Key: name of the key that was pressed, in lower case
%	Character: character interpretation of the key(s) that was pressed
%	Modifier: name(s) of the modifier key(s) (i.e., control, shift) pressed
% handles    structure with handles and user data (see GUIDATA)
switch eventdata.Key
    case 'uparrow'
        set(handles.kommando,'String','Fram')
        
        %%H�r vill vi l�gga kommandot f�r forward (0x01) i buffert
        fwrite(handles.BT, uint8(1));
        
        
    case 'downarrow'
        set(handles.kommando,'String','Back')
        
        %%H�r vill vi l�gga kommandot f�r backward (0x02) i buffert
        fwrite(handles.BT, uint8(2));
        
    case 'leftarrow'
        set(handles.kommando,'String','Rotera v�nster')
        
        %%H�r vill vi l�gga kommandot f�r rotate_left (0x06) i buffert
        fwrite(handles.BT, uint8(6));
        
    case 'rightarrow'
        set(handles.kommando,'String','Rotera h�ger')
        
        %%H�r vill vi l�gga kommandot f�r rotate_right (0x05) i buffert
        fwrite(handles.BT, uint8(5));
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
set(handles.bt_info, 'String', 'Wait...');
BT = Bluetooth('Koppel', 1); % Channel �r m�jligtvis inte alltid 1
disp('Bl�tansobjekt skapat.')
fopen(BT);
disp('Kommunikationskanal �ppnad.')
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
disp('Koppel fr�nkopplad')
set(handles.pushbutton3,'Enable','off')
set(handles.pushbutton2,'Enable','on')
set(handles.bt_info, 'String', 'Disonnected');
