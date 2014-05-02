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

% Last Modified by GUIDE v2.5 02-May-2014 13:57:29

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
handles.IR1_tid = IR1_tid; % Lägger till variabel i handles
handles.IR2_tid = IR2_tid;
handles.IR3_tid = IR3_tid;
handles.IR4_tid = IR4_tid;
handles.IR5_tid = IR5_tid;
handles.coord1 = coord1;
handles.coord2 = coord2;
%handles.bt = BT
% Övriga variabler från mjukvara.m skrivs här
guidata(hObject, handles); % Typ uppdaterar hObject med handles


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
axes(handles.axes1)
title('IR-värden över tid')
hold on
set(handles.ir_f,'String',20.7)
set(handles.ir_v_f,'String',3.1)
set(handles.ir_h_f,'String',5.2)
set(handles.ir_v_b,'String',8.0)
set(handles.ir_h_b,'String',4.4)

plot(handles.IR1_tid,'yellow')
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
    case 'downarrow'
        set(handles.kommando,'String','Back')
    case 'leftarrow'
        set(handles.kommando,'String','Rotera vänster')
    case 'rightarrow'
        set(handles.kommando,'String','Rotera höger')
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