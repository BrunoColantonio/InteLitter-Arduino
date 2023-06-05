package com.soa.intelitter;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity
{

    // Referencias de la interfaz grafica
    private Button buttonActivity;
    private TextView txtViewStateInteLitter;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Seteamos las referencias de la interfaz grafica
        buttonActivity = (Button) findViewById(R.id.buttonActivity);
        txtViewStateInteLitter = (TextView) findViewById(R.id.inteLitterState);

        // Agregamos un listener a cada una de las referencias
        buttonActivity.setOnClickListener((v) -> {
            // TODO: Enviar el estado del InteLitter a la segunda Activity.
            Intent intent = new Intent(MainActivity.this, SensorActivity.class);
            startActivity(intent);
        });

        // TODO: Leer por Bluetooth en que estado se encuentra el InteLitter
    }
}