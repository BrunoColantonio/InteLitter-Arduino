package com.soa.intelitter_soa;

import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

public class SensorActivity
        extends AppCompatActivity
        implements SensorEventListener 
{

    // Constants
    private static final float ACCELEROMETER_MAX_VALUE = 13;
    private static final int X_POSITION = 0;
    private static final int Y_POSITION = 1;
    private static final int Z_POSITION = 2;

    private SensorManager sensorManager;
    private Sensor sensor;
    private long lastUpdate, actualTime;

    private int STATE;

    private MediaPlayer mediaPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sensor);

        Intent intent = getIntent();
        STATE = intent.getIntExtra("state", InteLitter.STATE_CLEAN);

        lastUpdate = System.currentTimeMillis();
        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        sensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mediaPlayer = MediaPlayer.create(this, getMediaPlayerContent());

        mediaPlayer.setOnCompletionListener(
                new MediaPlayer.OnCompletionListener() 
                {
                    @Override
                    public void onCompletion(MediaPlayer mediaPlayer) 
                    {
                        mediaPlayer.stop();
                        mediaPlayer.release();
                    }
                }
        );

        if (sensor == null) 
        {
            Toast
                    .makeText(
                            this,
                            "No hay aceleremotro en tu dispositivo movil. :(",
                            Toast.LENGTH_SHORT
                    )
                    .show();
            finish();
        } 
        else 
        {
            sensorManager.registerListener(
                    this,
                    sensor,
                    SensorManager.SENSOR_DELAY_NORMAL
            );
        }
    }

    @Override
    protected void onStop() 
    {
        super.onStop();
        sensorManager.unregisterListener(this);
        if (mediaPlayer != null) 
        {
            mediaPlayer.release();
        }
    }

    @Override
    protected void onResume() 
    {
        super.onResume();
        // Solicitamos acceso a los sensores a utilizar
        sensorManager.registerListener(
                this,
                sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                SensorManager.SENSOR_DELAY_NORMAL
        );
    }

    @Override
    protected void onPause() 
    {
        sensorManager.unregisterListener(this);
        if (mediaPlayer != null) 
        {
            mediaPlayer.release();
        }
        super.onPause();
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) 
    {
        if (sensorEvent.sensor.getType() != Sensor.TYPE_ACCELEROMETER) return;
        if (isShaking(sensorEvent)) 
        {
            Log.i("MediaPlayer", "mediaPlayer.start()");
            mediaPlayer.start();
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {}

    private boolean isShaking(SensorEvent sensorEvent) 
    {
        return (
                Math.abs(sensorEvent.values[X_POSITION]) > ACCELEROMETER_MAX_VALUE ||
                        Math.abs(sensorEvent.values[Y_POSITION]) > ACCELEROMETER_MAX_VALUE ||
                        Math.abs(sensorEvent.values[Z_POSITION]) > ACCELEROMETER_MAX_VALUE
        );
    }

    private int getMediaPlayerContent() 
    {
        switch (STATE) 
        {
            case InteLitter.STATE_CLEAN:
                return R.raw.clean;
            case InteLitter.STATE_SLIGHTLY_DIRTY:
                return R.raw.s;
            case InteLitter.STATE_MID_DIRTY:
                return R.raw.m;
            case InteLitter.STATE_DIRTY:
                return R.raw.d;
            case InteLitter.STATE_CLEANING:
                return R.raw.cleaning;
            case InteLitter.STATE_CAT_INSIDE:
                return R.raw.c;
            default:
                break;
        }
        return R.raw.clean;
    }
}
