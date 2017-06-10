package com.dji.videostreamdecodingsample;
import android.util.Log;

import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfInt;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Timer;
import java.util.TimerTask;
/**
 * Created by Raul on 07/06/2017.
 */

public class SendVideoDroneTCP {

    private final int porta = 11006;
    private static ServerSocket socketVideoDrone;
    private static Socket connectionSocket;
    private static DataOutputStream outputVideoSocket;
    private static BufferedReader inputVideoSocket;
    private static DataInputStream inputVideoSocketStream;
    //private static Scanner s;
    private static byte[] frameBytes;
    private static int size;
    private static final String TAG = "SendVideoDroneTCP";

    public SendVideoDroneTCP(){
        try {
            socketVideoDrone = new ServerSocket(porta);
            Thread threadProcurarConexao = new Thread(new procurarConexaoThread());
            threadProcurarConexao.start();
        }catch(IOException io){
            Log.d(TAG, "Falha na criacao do socet;");
        }
    }

    public void procurarConexao(){
        Thread threadProcurarConexao = new Thread(new procurarConexaoThread());
        threadProcurarConexao.start();
    }

    private class procurarConexaoThread extends Thread{

        @Override
        public void run(){
            Log.d(TAG, "Procurando conexao...");
            try{
                connectionSocket = socketVideoDrone.accept();
                outputVideoSocket = new DataOutputStream(connectionSocket.getOutputStream());
                //inputVideoSocket = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
                inputVideoSocketStream = new DataInputStream(connectionSocket.getInputStream());
                //s = new Scanner(connectionSocket.getInputStream());

                Log.d(TAG, "Achou conexao");
            }catch(IOException io){
                Log.d(TAG, "Erro ao aceitar cliente tcp");
            }
        }
    }

    public void atualizarByteArray(byte[] data, int sizeR){

        Thread threadAtualizarArray = new Thread(new atualizarByteArrayThread(data, sizeR));
        threadAtualizarArray.start();
    }

    private class atualizarByteArrayThread extends Thread{

        public atualizarByteArrayThread(byte[] temp, int sizeR){
            frameBytes = temp.clone();
            temp = null;
            size = sizeR;
        }
        @Override
        public void run(){

        }

    }

    public void enviarByteArray(){
        Thread enviarBytesThread = new Thread(new enviarByteArrayClass());
        enviarBytesThread.start();
    }

    private class enviarByteArrayClass extends Thread{

        @Override
        public void run(){

            byte[] tempByte = frameBytes.clone();

            try {
                outputVideoSocket.write(tempByte.length);
                outputVideoSocket.write(tempByte);
                Log.d(TAG, "Enviado "+tempByte.length);

            }catch(IOException io){
                Log.d(TAG, "Erro no envio dos frames");
            }
            tempByte = null;
        }
    }

}
