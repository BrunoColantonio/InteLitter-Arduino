package com.soa.intelitter_soa;

import android.Manifest;
import android.annotation.SuppressLint;
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
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity 
{

    // Bluetooth Variables
    private BluetoothAdapter bluetoothAdapter_G;
    private BluetoothDevice bluetoothDevice_G;
    private BluetoothSocket bluetoothSocket_G;
    private OutputStream outputStream_G;
    private InputStream inputStream_G;

    // Component Variables
    private TextView stateTextView_G;
    private Button cleanButton_G;
    private Button sensorActivityButton_G;

    // Aux variables
    private boolean isStopped_G = false;
    private int state_G = InteLitter.STATE_CLEAN;

    // On Click Listener functions
    View.OnClickListener cleanFunction_G;
    View.OnClickListener sensorActivityFunction_G;

    // Handlers for Threads
    Handler handlerStateTextView_G = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        _InitBluetooth();
        _InitComponentVariables();
        _CreateBluetoothThread();
    }

    // Bluetooth Functions
    private void _InitBluetooth() 
    {

        bluetoothAdapter_G = BluetoothAdapter.getDefaultAdapter();

        if (bluetoothAdapter_G == null) 
        {
            Toast.makeText(this, "Este dispositivo no tiene Bluetooth :(", Toast.LENGTH_SHORT).show();
            finish();
        }

        if (!bluetoothAdapter_G.isEnabled()) 
        {
            Toast.makeText(this, "El Bluetooth esta desactivado :|", Toast.LENGTH_SHORT).show();
            finish();
        }

        if (bluetoothSocket_G != null) 
        {
            _GetBluetoothStreams();
        } 
        else 
        {
            _PairDevicesBluetooth();
        }
    }

    private void _GetBluetoothStreams() 
    {
        try 
        {
            outputStream_G = bluetoothSocket_G.getOutputStream();
            inputStream_G = bluetoothSocket_G.getInputStream();
        } catch (IOException e) 
        {
            Toast.makeText(this, "Error al obtener los stream del Bluetooth.", Toast.LENGTH_SHORT).show();
            finish();
        }
    }

    private void _PairDevicesBluetooth() 
    {
        _GetBluetoothDevice();
        _CreateBluetoothSocket();
        _ConnectBluetoothSocket();
    }

    private void _GetBluetoothDevice() 
    {
        int btPermission = ActivityCompat.checkSelfPermission(this, android.Manifest.permission.BLUETOOTH);
        int btAdminPermission = ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_ADMIN);

        if (btPermission != PackageManager.PERMISSION_GRANTED ||
                btAdminPermission != PackageManager.PERMISSION_GRANTED) 
        {
            return;
        }

        Set<BluetoothDevice> pairedDevices = bluetoothAdapter_G.getBondedDevices();

        for (BluetoothDevice device : pairedDevices) 
        {
            if (device.getName().equals("HC-05")) 
            {
                bluetoothDevice_G = device;
                break;
            }
        }
    }

    @SuppressLint("MissingPermission")
    private void _CreateBluetoothSocket() 
    {
        if (bluetoothDevice_G != null) 
        {
            UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
            try 
            {
                bluetoothSocket_G = bluetoothDevice_G.createRfcommSocketToServiceRecord(uuid);
            } catch (IOException e) 
            {
                Toast.makeText(this, "Error al crear el Socket de Bluetooth.", Toast.LENGTH_SHORT).show();
            }
        }
    }

    @SuppressLint("MissingPermission")
    private void _ConnectBluetoothSocket() 
    {
        if (bluetoothSocket_G != null) 
        {
            try 
            {
                bluetoothSocket_G.connect();
                outputStream_G = bluetoothSocket_G.getOutputStream();
                inputStream_G = bluetoothSocket_G.getInputStream();
            } catch (IOException e) 
            {
                Toast.makeText(this, "Error al conectar el Socket Bluetooth.", Toast.LENGTH_SHORT).show();
            }
        }
    }

    private boolean _SendMessageBluetooth(char msg) 
    {
        try 
        {
            outputStream_G.write((byte) msg);
        } 
        catch (IOException e) 
        {
            return false;
        }
        return true;
    }

    private void _CreateBluetoothThread() 
    {
        Thread thread = new Thread(this::_ReadBluetooth);
        thread.start();
    }

    private void _ReadBluetooth() 
    {
        byte[] buffer = new byte[1024];
        int bytesRead;

        while (true) 
        {
            try 
            {
                if (inputStream_G != null) 
                {
                    bytesRead = inputStream_G.read(buffer);
                    if (bytesRead != -1) 
                    {
                        String data = new String(buffer, 0, bytesRead);
                        handlerStateTextView_G.post(
                                () -> _HandleDataFromBluetooth(Integer.parseInt(data)));
                    }
                }
            } 
            catch (IOException e) 
            {
                Log.i("Error", e.getLocalizedMessage());
            }
        }
    }

    private void _HandleDataFromBluetooth(Integer value) 
    {
        if (InteLitter.isStateValue(value)) 
        {
            state_G = value;
            _HandleStateTextView(value);
        }
    }

    // Components Functions
    private void _InitComponentVariables() 
    {
        // Get the references from the UI
        stateTextView_G = findViewById(R.id.inteLitterState);
        cleanButton_G = findViewById(R.id.cleanButton);
        sensorActivityButton_G = findViewById(R.id.activityButton);

        // Set Listeners to the buttons.
        cleanFunction_G = view -> _Clean();
        cleanButton_G.setOnClickListener(cleanFunction_G);

        sensorActivityFunction_G = view -> _StartSensorActivity();
        sensorActivityButton_G.setOnClickListener(sensorActivityFunction_G);
    }

    private void _SetStateTextView(String txt, String hexColor) 
    {
        stateTextView_G.setText(txt);
        int color = Color.parseColor(hexColor);
        stateTextView_G.setBackgroundColor(color);
    }

    private void _HandleStateTextView(Integer data) 
    {
        switch (data) 
        {
            case InteLitter.STATE_CLEAN:
                _SetStateTextView("LIMPIO", LitterColors.BLUE);
                break;
            case InteLitter.STATE_SLIGHTLY_DIRTY:
                _SetStateTextView("POCO SUCIO", LitterColors.GREEN);
                break;
            case InteLitter.STATE_MID_DIRTY:
                _SetStateTextView("SUCIO", LitterColors.BROWN);
                break;
            case InteLitter.STATE_DIRTY:
                _SetStateTextView("MUY SUCIO", LitterColors.BLACK);
                break;
            case InteLitter.STATE_CLEANING:
                _SetStateTextView("LIMPIANDO", LitterColors.RED);
                break;
            case InteLitter.STATE_CAT_INSIDE:
                _SetStateTextView("GATO DENTRO", LitterColors.YELLOW);
                break;
            default:
                break;
        }
    }

    // Buttons Functions
    private void _Clean() 
    {
        _SendMessageBluetooth(isStopped_G ? 'C' : 'S');
        _SetIsStopped();
    }

    private void _StartSensorActivity() 
    {
        Intent intent = new Intent(MainActivity.this, SensorActivity.class);
        intent.putExtra("state", state_G);
        startActivity(intent);
    }

    // Auxs functions
    private void _SetIsStopped() 
    {
        isStopped_G = !isStopped_G;
        String txt = isStopped_G ? "LIMPIAR" : "DETENER";
        int color = Color.parseColor(
                isStopped_G ? LitterColors.GREEN : LitterColors.BLUE);
        cleanButton_G.setText(txt);
        cleanButton_G.setBackgroundColor(color);
    }
}
