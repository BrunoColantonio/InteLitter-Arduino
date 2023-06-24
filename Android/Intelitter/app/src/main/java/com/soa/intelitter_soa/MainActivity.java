package com.soa.intelitter_soa;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    // Bluetooth Variables
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothDevice bluetoothDevice;
    private BluetoothSocket bluetoothSocket;
    private OutputStream outputStream;
    private InputStream inputStream;

    // Component Variables
    private TextView stateTextView;
    private Button cleanButton;
    private Button sensorActivityButton;

    // Aux variables
    private boolean isStopped = false;
    private int STATE = InteLitter.STATE_CLEAN;

    // On Click Listener functions
    View.OnClickListener cleanFunction;
    View.OnClickListener sensorActivityFunction;

    // Handlers for Threads
    Handler handlerStateTextView = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initBluetooth();
        initComponentVariables();
        createBluetoothThread();
    }

    // Bluetooth Functions
    private void initBluetooth() {
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            // El dispositivo no es compatible con Bluetooth
            // Maneja esta situación según tus necesidades
        } else {
            if (!bluetoothAdapter.isEnabled()) {
                // Bluetooth esta desactivado.
                // Solicitamos al usuario que lo habilite.
            } else {
                // Bluetooth esta activado.
                // Preparamos el entorno para enviar y recibir datos.
                if (bluetoothSocket != null) {
                    try {
                        outputStream = bluetoothSocket.getOutputStream();
                        inputStream = bluetoothSocket.getInputStream();
                    } catch (IOException e) {}
                } else {
                    // Paso 1: Obtener el Bluetooth Device.
                    if (
                            ActivityCompat.checkSelfPermission(
                                    this,
                                    android.Manifest.permission.BLUETOOTH
                            ) !=
                                    PackageManager.PERMISSION_GRANTED ||
                                    ActivityCompat.checkSelfPermission(
                                            this,
                                            Manifest.permission.BLUETOOTH_ADMIN
                                    ) !=
                                            PackageManager.PERMISSION_GRANTED
                    ) {
                        // TODO: Consider calling
                        //    ActivityCompat#requestPermissions
                        // here to request the missing permissions, and then overriding
                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                        //                                          int[] grantResults)
                        // to handle the case where the user grants the permission. See the documentation
                        // for ActivityCompat#requestPermissions for more details.
                        return;
                    }
                    Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
                    for (BluetoothDevice device : pairedDevices) {
                        if (device.getName().equals("HC-05")) {
                            bluetoothDevice = device;
                            break;
                        }
                    }

                    // Paso 2: Crear el Bluetooth Socket
                    if (bluetoothDevice != null) {
                        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
                        try {
                            bluetoothSocket =
                                    bluetoothDevice.createRfcommSocketToServiceRecord(uuid);
                        } catch (IOException e) {}
                    }

                    // Paso 3: Conectar el Bluetooth Socket
                    if (bluetoothSocket != null) {
                        try {
                            bluetoothSocket.connect();
                            outputStream = bluetoothSocket.getOutputStream();
                            inputStream = bluetoothSocket.getInputStream();
                        } catch (IOException e) {}
                    }
                }
            }
        }
    }

    private boolean sendMessageBluetooth(char msg) {
        try {
            outputStream.write((byte) msg);
        } catch (IOException e) {
            return false;
        }
        return true;
    }

    private void createBluetoothThread() {
        Thread thread = new Thread(
                new Runnable() {
                    @Override
                    public void run() {
                        readBluetooth();
                    }
                }
        );

        thread.start();
    }

    private void readBluetooth() {
        byte[] buffer = new byte[1024];
        int bytesRead;

        while (true) {
            try {
                if (inputStream != null) {
                    bytesRead = inputStream.read(buffer);
                    if (bytesRead != -1) {
                        String data = new String(buffer, 0, bytesRead);
                        handlerStateTextView.post(
                                new Runnable() {
                                    @Override
                                    public void run() {
                                        handleDataFromBluetooth(Integer.parseInt(data));
                                    }
                                }
                        );
                    }
                }
            } catch (IOException e) {
                Log.i("Error", e.getLocalizedMessage());
                // break;
            }
        }
    }

    private void handleDataFromBluetooth(Integer value) {
        if (InteLitter.isStateValue(value)) {
            STATE = value;
            handleStateTextView(value);
            return;
        }
    }

    // Components Functions
    private void initComponentVariables() {
        // Get the references from the UI
        stateTextView = (TextView) findViewById(R.id.inteLitterState);
        cleanButton = (Button) findViewById(R.id.cleanButton);
        sensorActivityButton = (Button) findViewById(R.id.activityButton);

        // Set Listeners to the buttons.
        cleanFunction =
                new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        clean();
                    }
                };
        cleanButton.setOnClickListener(cleanFunction);

        sensorActivityFunction =
                new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        startSensorActivity();
                    }
                };
        sensorActivityButton.setOnClickListener(sensorActivityFunction);
    }

    private void setStateTextView(String txt, String hexColor) {
        stateTextView.setText(txt);
        int color = Color.parseColor(hexColor);
        stateTextView.setBackgroundColor(color);
    }

    private void handleStateTextView(Integer data) {
        switch (data) {
            case InteLitter.STATE_CLEAN:
                setStateTextView("LIMPIO", LitterColors.BLUE);
                break;
            case InteLitter.STATE_SLIGHTLY_DIRTY:
                setStateTextView("POCO SUCIO", LitterColors.GREEN);
                break;
            case InteLitter.STATE_MID_DIRTY:
                setStateTextView("SUCIO", LitterColors.BROWN);
                break;
            case InteLitter.STATE_DIRTY:
                setStateTextView("MUY SUCIO", LitterColors.BLACK);
                break;
            case InteLitter.STATE_CLEANING:
                setStateTextView("LIMPIANDO", LitterColors.RED);
                break;
            case InteLitter.STATE_CAT_INSIDE:
                setStateTextView("GATO DENTRO", LitterColors.YELLOW);
                break;
            default:
                break;
        }
    }

    // Buttons Functions
    private void clean() {
        sendMessageBluetooth(isStopped ? 'C' : 'S');
        setIsStopped();
    }

    private void startSensorActivity() {
        Intent intent = new Intent(MainActivity.this, SensorActivity.class);
        intent.putExtra("state", STATE);
        startActivity(intent);
    }

    // Auxs functions
    private void setIsStopped() {
        isStopped = !isStopped;
        String txt = isStopped ? "LIMPIAR" : "DETENER";
        int color = Color.parseColor(
                isStopped ? LitterColors.GREEN : LitterColors.BLUE
        );
        cleanButton.setText(txt);
        cleanButton.setBackgroundColor(color);
    }
}
