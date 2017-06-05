package com.dji.videostreamdecodingsample;

import android.util.Log;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Timer;
import java.util.TimerTask;

import dji.common.error.DJIError;
import dji.common.flightcontroller.virtualstick.FlightControlData;
import dji.common.util.CommonCallbacks;
import dji.sdk.flightcontroller.FlightController;

/**
 * Created by Raul on 17/05/2017.
 */

//classe para servir de interface com o computador
public class ServerDrone {

    //Variáveis de debug
    private static final String TAG = "ServerDrone";

    //Portas para o socket
    static final int socketServerPORT = 11001;
    static final int socketClientPORT = 11000;

    private SendVirtualStickDataTask mSendVirtualStickDataTask;

    private Timer mSendVirtualStickDataTimer;

    //Socket
    DatagramSocket serverSocketUDP;

    private FlightController mFlightController;
    private float mPitch;
    private float mRoll;
    private float mYaw;
    private float mThrottle;


    //construtor
    public ServerDrone(FlightController f){
        this.mFlightController = f;
        Thread ServerThread = new Thread(new ServerThread());
        ServerThread.start();
    }

    private class ServerThread extends Thread {

        InetAddress IPAddress;

        private void enviarMensagemUDP(String mensagem){

            String mensagemEnviada;
            mensagemEnviada = "Recebido "+mensagem;

            int msgTamanho = mensagemEnviada.length();
            byte[] transmitido = mensagemEnviada.getBytes();
            DatagramPacket p = new DatagramPacket(transmitido, msgTamanho, IPAddress, socketClientPORT);
            try {

                serverSocketUDP.send(p);
                //teste git
            }
            catch(IOException e){

            }
        }

        private void decolar(){
            if (mFlightController != null) {
                mFlightController.startTakeoff(
                        new CommonCallbacks.CompletionCallback() {
                            @Override
                            public void onResult(DJIError djiError) {
                                if (djiError != null) {

                                } else {

                                }
                            }
                        }
                );
            }
        }

        private void aterrisar(){
            if (mFlightController != null){

                mFlightController.startLanding(
                        new CommonCallbacks.CompletionCallback() {
                            @Override
                            public void onResult(DJIError djiError) {
                                if (djiError != null) {

                                } else {

                                }
                            }
                        }
                );

            }
        }

        @Override
        public void run() {

            try{

                serverSocketUDP = new DatagramSocket(socketServerPORT);

                //buffer
                byte[] recebido = new byte[1024];

                while(true){

                    Log.d(TAG, "Esperando o envio do peer!");

                    //Inicializa o DatagramPacket
                    DatagramPacket pacoteRecebido = new DatagramPacket(recebido, recebido.length);

                    //trava enquanto algo não for recebido na porta específica
                    serverSocketUDP.receive(pacoteRecebido);

                    //Obtem a string recebida
                    String pacoteString = new String(pacoteRecebido.getData(), 0, pacoteRecebido.getLength());

                    //obtêm o IP que enviou
                    IPAddress = pacoteRecebido.getAddress();

                    enviarMensagemUDP(pacoteString);

                    switch (pacoteString){

                        case "decolar":
                            decolar();
                            break;

                        case "aterrissar":
                            aterrisar();
                            break;

                        case "up":
                            mPitch = 0f;
                            mRoll = 0f;
                            mYaw = 0f;
                            mThrottle = 2f;
                            break;

                        case "down":
                            mPitch = 0f;
                            mRoll = 0f;
                            mYaw = 0f;
                            mThrottle = -2f;
                            break;

                        case "rollEsquerda":
                            mPitch = 0f;
                            mRoll = 4f;
                            mYaw = 0f;
                            mThrottle = 0f;
                            break;

                        case "rollDireita":
                            mPitch = 0f;
                            mRoll = -4f;
                            mYaw = 0f;
                            mThrottle = 0f;
                            break;

                        case "pitchUp":
                            mPitch = 3f;
                            mRoll = 0f;
                            mYaw = 0f;
                            mThrottle = 0f;
                            break;

                        case "pitchDown":
                            mPitch = -3f;
                            mRoll = 0f;
                            mYaw = 0f;
                            mThrottle = 0f;
                            break;

                        case "yawEsquerda":
                            mPitch = 0f;
                            mRoll = 0f;
                            mYaw = 4f;
                            mThrottle = 0f;
                            break;

                        case "yawDireita":
                            mPitch = 0f;
                            mRoll = 0f;
                            mYaw = -4f;
                            mThrottle = 0f;
                            break;

                        default:
                            mPitch = 0f;
                            mRoll = 0f;
                            mYaw = 0f;
                            mThrottle = 0f;
                            break;

                    }

                    if (null == mSendVirtualStickDataTimer) {
                        mSendVirtualStickDataTask = new SendVirtualStickDataTask();
                        mSendVirtualStickDataTimer = new Timer();
                        mSendVirtualStickDataTimer.schedule(mSendVirtualStickDataTask, 0, 200);
                    }

                }

            }catch(IOException e){

            }

        }

    }

    class SendVirtualStickDataTask extends TimerTask {

        @Override
        public void run() {

            if (mFlightController != null) {
                mFlightController.sendVirtualStickFlightControlData(
                        new FlightControlData(
                                mPitch, mRoll, mYaw, mThrottle
                        ), new CommonCallbacks.CompletionCallback() {
                            @Override
                            public void onResult(DJIError djiError) {
                                Log.d(TAG, "Dados enviados!");
                            }
                        }
                );
            }
        }
    }

}
