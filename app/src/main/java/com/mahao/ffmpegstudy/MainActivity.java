package com.mahao.ffmpegstudy;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    static {
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avformat-56");
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("swscale-3");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("postproc-53");
        System.loadLibrary("native-lib");
    }

    private Button tv;
    private MyVideoView videoView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tv = (Button) findViewById(R.id.sample_text);
        videoView = (MyVideoView) findViewById(R.id.my_surface);


    }

    public native String stringFromJNI();

    public native void open(String inputStr,String outStr);


    public void btnClick(View view) {

        tv.setText(stringFromJNI());

        String inputString = new File(Environment.getExternalStorageDirectory(),"mahao_0.mp4").getAbsolutePath();
        String outString = new File(Environment.getExternalStorageDirectory(),"myfuture.yuv").getAbsolutePath();
        open(inputString,outString);


        //播放视屏
        myPlay(view);

    }


    public void myPlay(View view){

        String inputString = new File(Environment.getExternalStorageDirectory(),"mahao_0.mp4").getAbsolutePath();
        videoView.player(inputString);
    }

}
