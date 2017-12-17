package com.dji.videostreamdecodingsample;

import android.Manifest;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.graphics.YuvImage;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.dji.videostreamdecodingsample.media.DJIVideoStreamDecoder;

import com.dji.videostreamdecodingsample.media.NativeHelper;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.core.Scalar;
import org.opencv.core.Size;

import dji.common.error.DJIError;
import dji.common.product.Model;
import dji.common.util.CommonCallbacks;
import dji.sdk.base.BaseProduct;
import dji.sdk.camera.VideoFeeder;
import dji.sdk.codec.DJICodecManager;
import dji.sdk.camera.Camera;
import dji.sdk.flightcontroller.FlightController;
import dji.sdk.products.Aircraft;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

import static org.opencv.core.CvType.CV_8UC1;
import static org.opencv.core.CvType.CV_8UC3;
import static org.opencv.core.CvType.CV_8UC4;
import static org.opencv.imgproc.Imgproc.COLOR_YUV2RGBA_NV21;
import static org.opencv.imgproc.Imgproc.COLOR_YUV2RGB_IYUV;
import static org.opencv.imgproc.Imgproc.COLOR_YUV2RGB_NV21;
import static org.opencv.imgproc.Imgproc.COLOR_YUV420sp2BGRA;
import static org.opencv.imgproc.Imgproc.cvtColor;


public class MainActivity extends Activity implements DJIVideoStreamDecoder.IYuvDataListener {
    private static final String TAG = MainActivity.class.getSimpleName();
    private static final String TAGDEBUG = "DebugOpencv";
    private static final String TAGALTERNATIVA = "other";
    private static int contador_imgs = 0;
    private static final String TAGOPENCV = "OpenCV";
    private ServerDrone server;
    public SendVideoDrone s;
    static final int MSG_WHAT_SHOW_TOAST = 0;
    static final int MSG_WHAT_UPDATE_TITLE = 1;
    static final boolean useSurface = true;

    private TextView titleTv;
    private TextureView videostreamPreviewTtView;
    private SurfaceView videostreamPreviewSf;
    private SurfaceHolder videostreamPreviewSh;

    private BaseProduct mProduct;
    private Camera mCamera;
    private DJICodecManager mCodecManager;

    private TextView savePath;
    private TextView screenShot;
    private List<String> pathList = new ArrayList<>();

    private HandlerThread backgroundHandlerThread;
    public Handler backgroundHandler;

    private FlightController mFlightController;

    /*Variáveis do Opencv*/
    private File mCascadeFile;
    private CascadeClassifier mJavaDetector;
    private int mAbsoluteFaceSize = 0;
    private float mRelativeFaceSize   = 0.1f;
    private double xCenter = -1;
    private double yCenter = -1;
    private static final Scalar    FACE_RECT_COLOR     = new Scalar(0, 255, 0, 255);

    protected VideoFeeder.VideoDataCallback mReceivedVideoDataCallBack = null;

    void enviarMat(Mat img){
        Mat imagem_rgb = img.clone();
        s.atualizarMat(imagem_rgb);
        s.sendMat();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (useSurface) {
            DJIVideoStreamDecoder.getInstance().resume();
        }
        Log.d(TAGOPENCV, "Tentando iniciar a execucao do opencv");
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_2_0, this,
                mLoaderCallback);
        initFlightController();
        notifyStatusChange();
    }

    @Override
    protected void onPause() {
        if (mCamera != null) {
            if (VideoFeeder.getInstance().getVideoFeeds() != null
                    && VideoFeeder.getInstance().getVideoFeeds().size() > 0) {
                VideoFeeder.getInstance().getVideoFeeds().get(0).setCallback(null);
            }
        }
        if (useSurface) {
            DJIVideoStreamDecoder.getInstance().stop();
        }
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        if (useSurface) {
            DJIVideoStreamDecoder.getInstance().destroy();
            NativeHelper.getInstance().release();
        }
        if (mCodecManager != null) {
            mCodecManager.destroyCodec();
        }
        super.onDestroy();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        NativeHelper.getInstance().init();

        // When the compile and target version is higher than 22, please request the
        // following permissions at runtime to ensure the
        // SDK work well.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.VIBRATE,
                            Manifest.permission.INTERNET, Manifest.permission.ACCESS_WIFI_STATE,
                            Manifest.permission.WAKE_LOCK, Manifest.permission.ACCESS_COARSE_LOCATION,
                            Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.ACCESS_FINE_LOCATION,
                            Manifest.permission.CHANGE_WIFI_STATE, Manifest.permission.MOUNT_UNMOUNT_FILESYSTEMS,
                            Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.SYSTEM_ALERT_WINDOW,
                            Manifest.permission.READ_PHONE_STATE,
                    }
                    , 1);
        }

        setContentView(R.layout.activity_main);

        backgroundHandlerThread = new HandlerThread("background handler thread");
        backgroundHandlerThread.start();
        backgroundHandler = new Handler(backgroundHandlerThread.getLooper());

        initUi();
        //intent para validar o drone
        IntentFilter filter = new IntentFilter();
        filter.addAction(DJISimulatorApplication.FLAG_CONNECTION_CHANGE);
        registerReceiver(mReceiver, filter);
        initPreviewer();


    }

    private void initFlightController() {

        Aircraft aircraft = DJISimulatorApplication.getAircraftInstance();
        if (aircraft == null || !aircraft.isConnected()) {
            showToast("Disconectado!");
            mFlightController = null;
            return;
        } else {
            mFlightController = aircraft.getFlightController();
        }

        if (mFlightController != null){

            mFlightController.setVirtualStickModeEnabled(true, new CommonCallbacks.CompletionCallback() {
                @Override
                public void onResult(DJIError djiError) {
                    if (djiError != null){
                        showToast(djiError.getDescription());
                    }else
                    {

                        showToast("Enable Virtual Stick Success");
                        server = new ServerDrone(mFlightController);
                    }
                }
            });

        }

    }

    protected BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            updateTitleBar();
        }
    };


    private void updateTitleBar() {
        if(titleTv == null) return;
        boolean ret = false;
        BaseProduct product = DJISimulatorApplication.getProductInstance();
        if (product != null) {
            if(product.isConnected()) {
                titleTv.setText("Drone Conectado!!!!!");
                ret = true;
            } else {
                if(product instanceof Aircraft) {
                    Aircraft aircraft = (Aircraft)product;
                    if(aircraft.getRemoteController() != null && aircraft.getRemoteController().isConnected()) {
                        // The product is not connected, but the remote controller is connected
                        titleTv.setText("only RC Connected");
                        ret = true;
                    }
                }
            }
        }

        if(!ret) {
            // The product or the remote controller are not connected.
            titleTv.setText("Disconnected");
        }
    }

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    Log.d(TAGOPENCV, "OpenCV loaded successfully");

                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(
                                R.raw.lbpcascade_frontalface);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFile = new File(cascadeDir,
                                "lbpcascade_frontalface.xml");
                        FileOutputStream os = new FileOutputStream(mCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();

                        mJavaDetector = new CascadeClassifier(
                                mCascadeFile.getAbsolutePath());
                        mJavaDetector.load(mCascadeFile.getAbsolutePath());
                        if (mJavaDetector.empty()) {
                            Log.d(TAGOPENCV,  mCascadeFile.getAbsolutePath());
                            mJavaDetector = null;
                        } else
                            Log.d(TAGOPENCV, "Loaded cascade classifier from "
                                    + mCascadeFile.getAbsolutePath());

                        cascadeDir.delete();
                        s = new SendVideoDrone();

                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.d(TAGOPENCV, "Failed to load cascade. Exception thrown: " + e);
                    }


                }
                break;
                default: {
                    Log.d(TAGOPENCV, "Erro no load do OpenCV");
                    super.onManagerConnected(status);

                }
                break;
            }
        }
    };

    public Handler mainHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_WHAT_SHOW_TOAST:
                    Toast.makeText(getApplicationContext(), (String) msg.obj, Toast.LENGTH_SHORT).show();
                    break;
                case MSG_WHAT_UPDATE_TITLE:
                    if (titleTv != null) {
                        titleTv.setText((String) msg.obj);
                    }
                    break;
                default:
                    break;
            }
        }
    };

    private void showToast(String s) {
        mainHandler.sendMessage(
                mainHandler.obtainMessage(MSG_WHAT_SHOW_TOAST, s)
        );
    }

    private void updateTitle(String s) {
        mainHandler.sendMessage(
                mainHandler.obtainMessage(MSG_WHAT_UPDATE_TITLE, s)
        );
    }

    private void initUi() {
        savePath = (TextView) findViewById(R.id.activity_main_save_path);
        screenShot = (TextView) findViewById(R.id.activity_main_screen_shot);
        screenShot.setSelected(false);
        titleTv = (TextView) findViewById(R.id.title_tv);
        videostreamPreviewTtView = (TextureView) findViewById(R.id.livestream_preview_ttv);
        videostreamPreviewSf = (SurfaceView) findViewById(R.id.livestream_preview_sf);
        videostreamPreviewSh = videostreamPreviewSf.getHolder();
        if (useSurface) {
            videostreamPreviewSf.setVisibility(View.VISIBLE);
            videostreamPreviewTtView.setVisibility(View.GONE);
            videostreamPreviewSh.addCallback(new SurfaceHolder.Callback() {
                @Override
                public void surfaceCreated(SurfaceHolder holder) {
                    DJIVideoStreamDecoder.getInstance().init(getApplicationContext(), videostreamPreviewSh.getSurface());
                    DJIVideoStreamDecoder.getInstance().setYuvDataListener(MainActivity.this);

                }

                @Override
                public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                    DJIVideoStreamDecoder.getInstance().changeSurface(holder.getSurface());
                }

                @Override
                public void surfaceDestroyed(SurfaceHolder holder) {

                }
            });
        } else {
            videostreamPreviewSf.setVisibility(View.GONE);
            videostreamPreviewTtView.setVisibility(View.VISIBLE);
        }

        DJIVideoStreamDecoder.getInstance().changeSurface(videostreamPreviewSh.getSurface());
    }

    private void notifyStatusChange() {

        mProduct = VideoDecodingApplication.getProductInstance();

        Log.d(TAG, "notifyStatusChange: " + (mProduct == null ? "Disconnect" : (mProduct.getModel() == null ? "null model" : mProduct.getModel().name())));
        if (mProduct != null && mProduct.isConnected() && mProduct.getModel() != null) {
            updateTitle(mProduct.getModel().name() + " Connected");
        } else {
            updateTitle("Disconnected");
        }

        // The callback for receiving the raw H264 video data for camera live view
        mReceivedVideoDataCallBack = new VideoFeeder.VideoDataCallback() {

            @Override
            public void onReceive(byte[] videoBuffer, int size) {

                Log.d(TAG, "camera recv video data size: " + size);
                if (useSurface) {
                    DJIVideoStreamDecoder.getInstance().parse(videoBuffer, size);
                } else if (mCodecManager != null) {
                    mCodecManager.sendDataToDecoder(videoBuffer, size);
                }

            }
        };

        if (null == mProduct || !mProduct.isConnected()) {
            mCamera = null;
            showToast("Disconnected");
        } else {
            if (!mProduct.getModel().equals(Model.UNKNOWN_AIRCRAFT)) {
                if (VideoFeeder.getInstance().getVideoFeeds() != null
                        && VideoFeeder.getInstance().getVideoFeeds().size() > 0) {
                    VideoFeeder.getInstance().getVideoFeeds().get(0).setCallback(mReceivedVideoDataCallBack);
                }
            }
        }
    }

    private void initPreviewer() {
        videostreamPreviewTtView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
                Log.d(TAG, "real onSurfaceTextureAvailable");
                if (mCodecManager == null) {
                    mCodecManager = new DJICodecManager(getApplicationContext(), surface, width, height);
                }
            }

            @Override
            public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            }

            @Override
            public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
                if (mCodecManager != null) mCodecManager.cleanSurface();
                return false;
            }

            @Override
            public void onSurfaceTextureUpdated(SurfaceTexture surface) {

            }
        });
    }

    @Override
    public void onYuvDataReceived(byte[] yuvFrame, int width, int height) {

        Log.d(TAGOPENCV, "recebeu yuv");
        Log.d(TAGDEBUG,DJIVideoStreamDecoder.VIDEO_ENCODING_FORMAT);

        if (DJIVideoStreamDecoder.getInstance().frameIndex % 30 == 0) {

            byte[] y = new byte[width * height];
            byte[] u = new byte[width * height / 4];
            byte[] v = new byte[width * height / 4];
            byte[] nu = new byte[width * height / 4];
            byte[] nv = new byte[width * height / 4];

            System.arraycopy(yuvFrame, 0, y, 0, y.length);

            for (int i = 0; i < u.length; i++) {
                v[i] = yuvFrame[y.length + 2 * i];
                u[i] = yuvFrame[y.length + 2 * i + 1];
            }

            int uvWidth = width / 2;
            int uvHeight = height / 2;
            for (int j = 0; j < uvWidth / 2; j++) {
                for (int i = 0; i < uvHeight / 2; i++) {
                    byte uSample1 = u[i * uvWidth + j];
                    byte uSample2 = u[i * uvWidth + j + uvWidth / 2];
                    byte vSample1 = v[(i + uvHeight / 2) * uvWidth + j];
                    byte vSample2 = v[(i + uvHeight / 2) * uvWidth + j + uvWidth / 2];
                    nu[2 * (i * uvWidth + j)] = uSample1;
                    nu[2 * (i * uvWidth + j) + 1] = uSample1;
                    nu[2 * (i * uvWidth + j) + uvWidth] = uSample2;
                    nu[2 * (i * uvWidth + j) + 1 + uvWidth] = uSample2;
                    nv[2 * (i * uvWidth + j)] = vSample1;
                    nv[2 * (i * uvWidth + j) + 1] = vSample1;
                    nv[2 * (i * uvWidth + j) + uvWidth] = vSample2;
                    nv[2 * (i * uvWidth + j) + 1 + uvWidth] = vSample2;
                }
            }

            byte[] bytes = new byte[yuvFrame.length];
            System.arraycopy(y, 0, bytes, 0, y.length);
            for (int i = 0; i < u.length; i++) {
                bytes[y.length + (i * 2)] = nv[i];
                bytes[y.length + (i * 2) + 1] = nu[i];
            }
            Mat myuv = new Mat(height + height / 2, width, CV_8UC1);
            myuv.put(0,0,bytes);

            Mat picBGR = new Mat(height, width, CV_8UC4);

            cvtColor(myuv, picBGR, Imgproc.COLOR_YUV2BGRA_NV21);
            enviarMat(picBGR);
        }

    }

    public void onClick(View v) {
        DJIVideoStreamDecoder.getInstance().changeSurface(null);
    }

}
