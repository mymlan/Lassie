clc;

%%%%%%%%%%% BLUETOOTHUPPKOPPLING %%%%%%%%%%%%%
%%%%%%%%%%% avkommentera det som är intressant %%%%%%
%info = instrhwinfo('Bluetooth');
%info.RemoteNames
%Lassie_info = instrhwinfo('Bluetooth', '00066602D47F')
% Device ID: 00066602D47F
% BT = Bluetooth('Koppel', 1); % Channel är möjligtvis inte alltid 1
% disp('Blåtansobjekt skapat.')
% fopen(BT);
% disp('Kommunikationskanal öppnad.')

%%%%%%%%%%% DUMMYDATA %%%%%%%%%%%%%%%%%%%%%%%
IR1 = 2.3; IR2 = 0.3; IR3 = 5.2; IR4 = 12.5; IR5 = 6.0;

%%%%%%%%%%%%%%%%% ÖPPNAR GUI %%%%%%%%%%%%%%%%%%
mjukvaru_gui