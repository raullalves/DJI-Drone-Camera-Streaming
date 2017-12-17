package com.dji.videostreamdecodingsample;
import android.util.Log;

import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfInt;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.util.Arrays;
import java.util.Timer;
import java.util.TimerTask;

//import static com.dji.videostreamdecodingsample.SendVideoDrone.SendByteDataDroneThread.BUFFER_SIZE;
import static org.opencv.imgproc.Imgproc.INTER_LINEAR;

/**
 * Created by Raul on 27/05/2017.
 */

public class SendVideoDrone {

    private static byte[] byteDataTeste;
    private static Mat img_enviar;
    private static int size;
    private static final String TAG = "SendVideoDrone";
    static final int socketServerPORT = 11006;
    static DatagramSocket socketVideoDrone;
    public static InetAddress IPAddress;
    public SendVideoDrone(){
        Thread getACK = new Thread(new receiveACK());
        getACK.start();
    }

    public void atualizarMat(Mat mat){
        Thread atualizarMatThread = new Thread(new AtualizarMatThread(mat));
        atualizarMatThread.start();
    }

    private class AtualizarMatThread extends Thread{

        public AtualizarMatThread(Mat img){
            img_enviar = img.clone();
        }

        @Override
        public void run(){

        }
    }

    private class atualizarByteArrayThread extends Thread{
        public atualizarByteArrayThread(byte[] temp, int sizeR){
            byteDataTeste = temp.clone();
            size = sizeR;
        }
        @Override
        public void run(){

        }
    }

    public void sendMat(){
        Thread SendVideoThread = new Thread(new SendVideoDroneThread( this.IPAddress));
        SendVideoThread.start();

    }

    private class receiveACK extends Thread{
        @Override
        public void run() {

            try{
                socketVideoDrone = new DatagramSocket(socketServerPORT);
            }
            catch(IOException e){
                Log.d(TAG,"Erro ao criar o socket");
                return;
            }


            byte[] recebido = new byte[1024];
            DatagramPacket pacoteRecebido = new DatagramPacket(recebido, recebido.length);
            InetAddress tempIP;
            try{
                socketVideoDrone.receive(pacoteRecebido);
                Log.d(TAG, "ACK recebido");
                tempIP = pacoteRecebido.getAddress();
                Log.d(TAG, tempIP.toString());
            }catch(IOException e){
                Log.d(TAG, "Erro ao receber o ack");
                return;
            }
            IPAddress = tempIP;
            Log.d(TAG, IPAddress.toString());
        }
    }

    public void sendByteData(){
        Thread SendVideoThread = new Thread(new SendByteDataDroneThread(this.IPAddress));
        SendVideoThread.start();
    }

    private class SendByteDataDroneThread extends Thread{
        static final int UDP_PACK_SIZE = 4096;
        private static final int BUFFER_SIZE = 1024;
        private byte[] byteData;
        private int sizeEnviar;
        InetAddress ip;
        byte[] ack = new byte[1024];

        public SendByteDataDroneThread(byte[] dataReceived, int size,InetAddress ip){
            this.byteData = dataReceived.clone();
           // this.size = size;
            dataReceived = null;
            this.ip = ip;
        }

        public SendByteDataDroneThread(InetAddress ip){
            this.ip = ip;
        }

        @Override
        public void run(){
            this.sizeEnviar = size;
            int total_enviar = 1 + (this.sizeEnviar-1) / UDP_PACK_SIZE;
            Log.d(TAG, "ENviado "+total_enviar);
            DatagramPacket ackRecebido = new DatagramPacket(ack, ack.length);

            Integer tamanho = total_enviar;
            byte[] sendData = new byte[ BUFFER_SIZE ];
            sendData = ByteBuffer.allocate(4).putInt( tamanho ).array();
            DatagramPacket p = new DatagramPacket(sendData, sendData.length , this.ip, socketServerPORT);
            String pacoteString;

            try{
                socketVideoDrone.send(p);
                do {
                    socketVideoDrone.receive(ackRecebido);
                    pacoteString = new String(ackRecebido.getData(), 0, ackRecebido.getLength());
                }while(!pacoteString.equals(Integer.toString(total_enviar)));

            }catch(IOException ex){
                Log.d(TAG, "Erro no envio do int");
                return;
            }

            byte[] tempByte = byteDataTeste.clone();
            //DatagramPacket ackRecebidoSeq = new DatagramPacket(ack, ack.length);
            for (int i = 1; i <= total_enviar; i++){

                byte[] slice = Arrays.copyOfRange( tempByte, UDP_PACK_SIZE*(i-1), UDP_PACK_SIZE*(i));
                DatagramPacket framePacket = new DatagramPacket(slice, UDP_PACK_SIZE , this.ip, socketServerPORT);
                DatagramPacket ackRecebidoSeq = new DatagramPacket(ack, ack.length);
                String pacoteString2;
                try{
                    do{
                        socketVideoDrone.send(framePacket);
                        socketVideoDrone.receive(ackRecebidoSeq);
                        pacoteString2 = new String(ackRecebidoSeq.getData(), 0, ackRecebidoSeq.getLength());
                    }while(!pacoteString2.equals(Integer.toString(i)));

                }catch(IOException e){
                    Log.d(TAG, "Erro ao enviar o frame");
                    return;
                }

            }

        }
    }

    private class SendVideoDroneThread extends Thread {

        static final int FRAME_HEIGHT = 480;
        static final int FRAME_WIDTH = 640;
        static final double FRAME_INTERVAL = (1000/30);
        static final int UDP_PACK_SIZE = 4096;
        static final int ENCODE_QUALITY = 60;
        static final int TAMANHO_BUFFER = 65540;
        static final int BUFFER_SIZE_B = 2048;
        Mat mat;
        Size size;
        InetAddress ip;
        public SendVideoDroneThread(InetAddress ip){
            //this.mat = img.clone();
            this.size = new Size(FRAME_WIDTH, FRAME_HEIGHT);
            this.ip = ip;
        }
        @Override
        public void run() {
            byte[] ack = new byte[1024];
            DatagramPacket ackRecebido = new DatagramPacket(ack, ack.length);
            MatOfByte encoded = new MatOfByte();

            MatOfInt params = new MatOfInt(Imgcodecs.IMWRITE_JPEG_QUALITY, ENCODE_QUALITY);
            Mat img_temp = img_enviar.clone();
            Imgproc.resize( img_temp, img_temp, size, 0, 0,INTER_LINEAR );
            Imgcodecs.imencode(".jpg", img_temp, encoded, params);
            int total_enviar = 1 + ((int)encoded.size().area() - 1) / UDP_PACK_SIZE;
            Integer tamanho = total_enviar;
            byte[] sendData = new byte[ BUFFER_SIZE_B ];
            sendData = ByteBuffer.allocate(4).putInt( tamanho ).array();

            DatagramPacket p = new DatagramPacket(sendData, sendData.length , this.ip, socketServerPORT);
            String pacoteString;
            try{

                socketVideoDrone.send(p);
                do {

                    socketVideoDrone.receive(ackRecebido);
                    pacoteString = new String(ackRecebido.getData(), 0, ackRecebido.getLength());
                }while(!pacoteString.equals(Integer.toString(total_enviar)));

            }catch(IOException ex){
                Log.d(TAG, "Erro no envio do int");
                return;
            }

            for (int i = 1; i <= total_enviar; i++){
                byte[] frameArray = encoded.toArray();
                byte[] slice = Arrays.copyOfRange(frameArray, UDP_PACK_SIZE*(i-1), UDP_PACK_SIZE*(i));
                DatagramPacket frame = new DatagramPacket(slice, UDP_PACK_SIZE , IPAddress, socketServerPORT);
                try{
                    socketVideoDrone.send(frame);

                }catch(IOException e){
                    Log.d(TAG, "Erro ao enviar o frame");
                    return;
                }
            }
            Log.d(TAG, "Frame enviado");

        }

    }
}
