clc;

%%%%%%%%%%% BLUETOOTHUPPKOPPLING %%%%%%%%%%%%%
%%%%%%%%%%% avkommentera det som är intressant %%%%%%
 %info = instrhwinfo('Bluetooth');
 % info.RemoteNames
%  % Lassie_info = instrhwinfo('Bluetooth', '00066602D47F')
% %  %Device ID: 00066602D47F
 %  BT = Bluetooth('FireFly-D47F', 1); % Channel är möjligtvis inte alltid 1
%   %disp('Blåtansobjekt skapat.')
 % fopen(BT);
%  disp('Kommunikationskanal öppnad.')
% fclose(BT);
%%%%%%%%%%% DUMMYDATA %%%%%%%%%%%%%%%%%%%%%%%
IR1 = 2.3; IR2 = 0.3; IR3 = 5.2; IR4 = 12.5; IR5 = 6.0;

IR1_tid = [5.3 5.6 5.8 5.8 6.7 6.5];
IR2_tid = [2.1 2.2 2.8 2.8 2.7 3.2];
IR3_tid = [2.3 2.6 2.8 3.8 2.7 3.5];
IR4_tid = [2.3 2.6 2.8 1.8 2.7 3.7];
IR5_tid = [2.3 2.6 2.8 1.5 2.7 2.5];

coord1 = [0 0 2 2 5 5 6 3 5 5 1];
coord2 = [0 3 3 1 1 5 5 5 5 1 1];

save('mjukvaruvariabler')
%%%%%%%%%%%%%%%%% ÖPPNAR GUI %%%%%%%%%%%%%%%%%%
mjukvaru_gui()