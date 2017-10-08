package com.mahao.ffmpegstudy;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by mahao on 2017/10/7.
 */

public class MyVideoView extends SurfaceView {


    public MyVideoView(Context context) {
        this(context,null);
    }

    public MyVideoView(Context context, AttributeSet attrs) {
        this(context, null,0);
    }

    public MyVideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {

        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
    }

    public void player(final String input){

        new Thread(new Runnable() {

            @Override
            public void run() {

                render(input,getHolder().getSurface());

            }
        }).start();
    }

    public native void render(String input, Surface surface);

}







