

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
axes(handles.axes1)
title('IR-värden över tid')
hold on
set(handles.ir_f,'String',20.7)
set(handles.ir_v_f,'String',3.1)
set(handles.ir_h_f,'String',5.2)
set(handles.ir_v_b,'String',8.0)
set(handles.ir_h_b,'String',4.4)

% get_sensor_data = true;
% 
% ID_BYTE_IR_SENSOR_DATA = 1;
% ID_BYTE_DISTANCE = 3;
% ID_BYTE_AUTO_DECISIONS = 8;
% 
% number_of_sensor_data_collected = 0;
% number_of_distance_collected = 0;
% % För att göra processen så effektiv som möjligt är det bättre att allokera
% % minne till alla vektorer innan loopen så de inte behöver växa under
% % tiden. Om vi kör roboten i 20 min = 1200 sek = 1200000 ms och hämtar
% % sensordata var 40e ms kommer vi få arayer som är 30000 värden långa.
% IR_infront = zeros(1,50000);
% IR_right_front = zeros(1,50000);
% IR_right_back = zeros(1,50000);
% IR_left_front = zeros(1,50000);
% IR_left_back = zeros(1,50000);
% regulator_error = zeros(1,50000);
% regulator_angle = zeros(1,50000);
% distance = zeros(1,50000);
% 
% 
% while get_sensor_data
%     id_byte = fread(handles.BT); 
%     
%   %for j = 1:1 %denna forloop är för testning 
%       
%     switch id_byte
%         case ID_BYTE_IR_SENSOR_DATA
%             number_of_sensor_data_collected = number_of_sensor_data_collected + 1;
%             number_of_bytes_in_sensor_data = 7; 
%             for i = 1:number_of_bytes_in_sensor_data
%                 byte = fread(handles.BT);
%                 switch i
%                     case 7
%                         regulator_error(number_of_sensor_data_collected) = byte; 
%                     case 6
%                         regulator_angle(number_of_sensor_data_collected) = byte;  
%                     case 5
%                         IR_infront(number_of_sensor_data_collected) = byte; 
%                     case 4
%                         IR_right_back(number_of_sensor_data_collected) = byte; 
%                     case 3
%                         IR_right_front(number_of_sensor_data_collected) = byte; 
%                     case 2
%                         IR_left_back(number_of_sensor_data_collected) = byte; 
%                     case 1
%                         IR_left_front(number_of_sensor_data_collected) = byte; 
%                 end
%             end
%         case ID_BYTE_DISTANCE
%           number_of_distance_collected = number_of_distance_collected + 1;
%           byte = fread(handles.BT);
%           distance(number_of_distance_collected) = byte;
%         case ID_BYTE_AUTO_DECISIONS
%           byte = fread(handles.BT); 
%           %Kan ha en textruta där det står kanske
%     end
%     
%     %Tanken var att jag inte behöver plotta hela vektorn utan bara de 100
%     %senaste värdena. Hur ska jag då plotta alla i samma ruta
%     start_value_of_plot_array = 1
%      if(number_of_sensor_data_collected > 100)
%          start_value_of_plot_array = number_of_sensor_data_collected - 100;
%      end
%     IR_infront_plot = IR_infront(start_value_of_plot_array:number_of_sensor_data_collected);
%     IR_right_front_plot = IR_right_front(start_value_of_plot_array:number_of_sensor_data_collected);
%     IR_right_back_plot = IR_right_back(start_value_of_plot_array:number_of_sensor_data_collected);
%     IR_left_front_plot = IR_left_front(start_value_of_plot_array:number_of_sensor_data_collected);
%     IR_left_back_plot = IR_left_back(start_value_of_plot_array:number_of_sensor_data_collected);
%     
%     axes(handles.axes1)
%     title('IR-värden över tid')
%     set(handles.ir_f,'String',20.7)
%     set(handles.ir_v_f,'String',3.1)
%     set(handles.ir_h_f,'String',5.2)
%     set(handles.ir_v_b,'String',8.0)
%     set(handles.ir_h_b,'String',4.4)
%     
%     hold off 
%     plot(IR_left_back_plot,'black')
%     axes(handles.axes4)
%     plot(IR_left_front_plot,'blue')
%     axes(handles.axes5)
%     plot(IR_right_back_plot,'green')
%     axes(handles.axes6)
%     plot(IR_infront_plot,'yellow')  
%     axes(handles.axes7)
%     plot(IR_right_front_plot,'red')
%     
%   %end 
%     
%    get_sensor_data = false;       
% end

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
    case 'w'
        set(handles.kommando,'String','Fram') 
        %Kommandot för forward är 0x01
        fwrite(handles.BT, uint8(1));       
    case 's'
        set(handles.kommando,'String','Back')
        %Kommandot för backward är 0x02
        fwrite(handles.BT, uint8(2));  
    case 'aeee'
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
%fwrite(handles.BT, uint8(0));

% ---
% function Bytes_available_in_buffer(handles)
% fprintf(handles.BT,'*IDN?')
%     %byte = fread(handles.BT);
    

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
BT = Bluetooth('Koppel', 1); % Channel är möjligtvis inte alltid 1
disp('Blåtansobjekt skapat.')
% BT.BytesAvailableFcnCount = 1;
% BT.BytesAvailableFcnMode = 'byte';
% BT.BytesAvailableFcn = {'Bytes_available_in_buffer', handles};
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
