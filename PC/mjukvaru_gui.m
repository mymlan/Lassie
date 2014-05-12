

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

% Last Modified by GUIDE v2.5 09-May-2014 14:03:39

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

% Övriga variabler från mjukvara.m skrivs här
guidata(hObject, handles); % Typ uppdaterar hObject med handles


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
axes(handles.ir_front_long)
title('IR-värden över tid')
hold on
set(handles.ir_f,'String',20.7)
set(handles.ir_v_f,'String',3.1)
set(handles.ir_h_f,'String',5.2)
set(handles.ir_v_b,'String',8.0)
set(handles.ir_h_b,'String',4.4)

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
set(handles.styrform, 'String', 'Manuell')

switch eventdata.Key
    case 'w'
        set(handles.kommando,'String','Fram') 
        %Kommandot för forward är 0x01
        fwrite(handles.BT, uint8(1));       
    case 's'
        set(handles.kommando,'String','Back')
        %Kommandot för backward är 0x02
        fwrite(handles.BT, uint8(2));  
    case 'a'
        set(handles.kommando,'String','Rotera vänster') 
        %Kommandot för rotate_left är 0x06
        fwrite(handles.BT, uint8(6));       
    case 'd'
        set(handles.kommando,'String','Rotera höger')
        %Kommandot för rotate_right är 0x05
        fwrite(handles.BT, uint8(5));
    case 'q'
        set(handles.kommando,'String','Sväng vänster')
        %Kommandot för turn_left är 0x04
        fwrite(handles.BT, uint8(4));
    case 'e'
        set(handles.kommando,'String','Sväng höger')
        %Kommandot för turn_right är 0x03
        fwrite(handles.BT, uint8(3)); 
    case 'space'
        set(handles.kommando,'String','Stanna')
        %Kommandot för stop är 0x00
        fwrite(handles.BT, uint8(0)); 
    case 'x'
        set(handles.kommando,'String','greppa med gripklo')
        %Kommandot för open_claw är 0x08
        fwrite(handles.BT, uint8(8)); 
    case 'z'
        set(handles.kommando,'String','öppna gripklo')
        %Kommandot för close_claw är 0x07
        fwrite(handles.BT, uint8(7)); 
        
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

% ---
%  function Bytes_available_in_buffer(handles)
%  byte = fread(handles.BT);
%  disp('test')
   
    

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
BT = Bluetooth('FireFly-D47F', 1); % Channel är möjligtvis inte alltid 1
disp('Blåtansobjekt skapat.')
% BT.BytesAvailableFcnCount = 1;
% BT.BytesAvailableFcnMode = 'byte';
% BT.BytesAvailableFcn = {'Bytes_available_in_buffer', handles};
%set(BT, 'Timeout', 30);
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


% --- Executes on button press in pushbutton4.
function pushbutton4_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
IR_front_long = zeros(1,1000);
IR_right_front = zeros(1,1000);
IR_right_back = zeros(1,1000);
IR_left_front = zeros(1,1000);
IR_left_back = zeros(1,1000);
regulator_error = zeros(1,1000);
regulator_angle = zeros(1,1000);

disp('bytes available')
disp(handles.BT.BytesAvailable)

for i = 1:60
while handles.BT.BytesAvailable == 0
    %disp('no bytes available')
end
id_byte = fread(handles.BT,1);
disp(id_byte)
switch id_byte 
  case 1 %sensorvärden
        for j = 1:7
            %Ev lägga in en while här, men känns onödigt om felmeddelandet
            %aldrig uppstår
           byte = fread(handles.BT,1);
            switch j
                case 1
                    regulator_error(i) = byte;
                    set(handles.error,'String',byte)
                    drawnow()
                case 2
                    regulator_angle(i) = byte;
                    set(handles.angle,'String',byte)
                    drawnow()
                case 3
                    IR_front_long(i) = byte;
                    set(handles.ir_f,'String',byte)
                    drawnow()
                case 4
                    IR_right_back(i) = byte;
                    set(handles.ir_h_f,'String',byte)
                    drawnow()
                case 5
                    IR_right_front(i) = byte;
                    set(handles.ir_h_b,'String',byte)
                    drawnow()
                case 6
                    IR_left_back(i) = byte;
                    set(handles.ir_v_b,'String',byte)
                    drawnow()
                case 7
                    IR_left_front(i) = byte;
                    set(handles.ir_v_f,'String',byte)
                    drawnow()
                otherwise 
                    disp('error i switch för sensordata')
            end
        end
    case 3 %distance
        byte = fread(handles.BT,1);
        set(handles.distance,'String',byte)
    case 8 %auto_decisions
        set(handles.styrform, 'String', 'Autonom')
        byte = fread(handles.BT,1);
        %disp(byte)
        switch byte
            case 0 %Stop
                set(handles.kommando, 'String', 'Stanna')
                drawnow() %Tvingar GUI att uppdatera sig
            case 1 %Forward
                set(handles.kommando, 'String', 'Fram')
                drawnow()
            case 2 %Backward
                set(handles.kommando, 'String', 'Back')
                drawnow()
            case 3 %Turn Right
                set(handles.kommando, 'String', 'Sväng höger')
                drawnow()
            case 4 %Turn left
                set(handles.kommando, 'String', 'Sväng vänster')
                drawnow()
            case 5 %Rotate Right
                set(handles.kommando, 'String', 'Rotera höger')
                drawnow()
            case 6 %Rotera vänster
                set(handles.kommando, 'String', 'Rotera vänster')
                drawnow()
            case 7 %Close claw
                set(handles.kommando, 'String', 'Greppa med gripklo')
                drawnow()
            case 8 %Open claw
                set(handles.kommando, 'String', 'öppna gripklo')
                drawnow()
            case 9 %Forward not regulated
                set(handles.kommando, 'String', 'Fram, ej reglerad')
                drawnow()
            case 10 %Backward not regulated
                set(handles.kommando, 'String', 'Back, ej reglerad')
                drawnow()
            case 11 %Tight turn right
                set(handles.kommando, 'String', 'Snäv högersväng')
                drawnow()   
            case 12 %Tight turn left
                set(handles.kommando, 'String', 'Snäv vänstersväng')
                drawnow() 
            otherwise
                disp('error i auto_decisions')
        end
    case 12 %Map coordinates
        disp('uppdate map')
        %Hur vill vi representera kartan
        %x_coordinate = fread(handles.BT,1);
        %y_coordinate = fread(handles.BT,1);
    case 13 %Map parameters
        for k = 1:7
          byte = fread(handles.BT,1);
          switch k
                case 1
                    set(handles.robot_direction,'String',byte)
                    drawnow()
                case 2
                    set(handles.size_all_nodes,'String',byte)
                    drawnow()
                case 3
                    set(handles.following_path,'String',byte)
                    drawnow()
                case 4
                    set(handles.enable_node_edit,'String',byte)
                    drawnow()
                case 5
                    set(handles.level,'String',byte)
                    drawnow()
                case 6
                    set(handles.x_coordinate,'String',byte)
                    drawnow()
                case 7
                    set(handles.y_coordinate,'String',byte)
                    drawnow()
                otherwise 
                    disp('error i switch för kartparametrar')
          end
        end
    otherwise
        disp('error i switch för id-byte')
end

start_value_to_plot_array = 1;
if(size(IR_front_long)> 100)
    start_value_to_plot_array = size(IR_front_long) - 100;
end
axes(handles.ir_front_long)
plot(IR_front_long(start_value_to_plot_array:size(IR_front_long)))
axes(handles.ir_right_back)
plot(IR_right_back(start_value_to_plot_array:size(IR_right_back)))
axes(handles.ir_right_front)
plot(IR_right_front(start_value_to_plot_array:size(IR_right_front)))
axes(handles.ir_left_back)
plot(IR_left_back(start_value_to_plot_array:size(IR_right_front)))
axes(handles.ir_left_front)
plot(IR_left_front(start_value_to_plot_array:size(IR_left_front)))
drawnow()
end

% disp(IR_left_front)
% disp(IR_left_back)
% disp(IR_right_front)
% disp(IR_right_back)
% disp(IR_front_long)
% disp(regulator_angle)
% disp(regulator_error)
disp('all data du ville hämta är hämtad')

