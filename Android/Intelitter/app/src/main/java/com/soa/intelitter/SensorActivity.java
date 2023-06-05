package com.soa.intelitter;

import androidx.appcompat.app.AppCompatActivity;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.MediaPlayer;
import android.os.Bundle;

public class SensorActivity extends AppCompatActivity implements SensorEventListener
{
    // Constants
    private final static float ACCELEROMETER_MAX_VALUE = 30;
    private final static int X_POSITION = 0;
    private final static int Y_POSITION = 1;
    private final static int Z_POSITION = 2;

    // Attributes
    private SensorManager mSensorManager;
    private MediaPlayer mediaPlayer;
    // TODO: Crear mas media player para poder reproducir distintas canciones segun el estado del InteLitter

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sensor);

        // Get an instance of Sensor Manager
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);

        // Creates Media Player Instance
        mediaPlayer = MediaPlayer.create(this, R.raw.masRicaQueAyer);
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent)
    {
        if (sensorEvent.sensor.getType() != Sensor.TYPE_ACCELEROMETER) return;

        if (
                Math.abs(sensorEvent.values[X_POSITION]) > ACCELEROMETER_MAX_VALUE ||
                        Math.abs(sensorEvent.values[Y_POSITION]) > ACCELEROMETER_MAX_VALUE ||
                        Math.abs(sensorEvent.values[Z_POSITION]) > ACCELEROMETER_MAX_VALUE
        )
        {
            // TODO: Reproducir una cancion segun el estado en el que se encuentre el InteLitter
            mediaPlayer.start();
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i)
    {

    }

    @Override
    protected void onResume() {
        super.onResume();
        // Solicitamos acceso a los sensores a utilizar
        mSensorManager.registerListener(
                this,
                mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
                SensorManager.SENSOR_DELAY_NORMAL
        );
    }

    @Override
    protected void onStop() {
        mSensorManager.unregisterListener(this);
        if (mediaPlayer != null)
        {
            mediaPlayer.release();
        }
        super.onStop();
    }

    @Override
    protected void onPause() {
        mSensorManager.unregisterListener(this);
        if (mediaPlayer != null)
        {
            mediaPlayer.release();
        }
        super.onPause();
    }
}