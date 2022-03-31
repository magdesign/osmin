package io.github.janbar.osmin;

import android.content.Context;
import android.content.Intent;
import android.util.Log;
import org.qtproject.qt5.android.bindings.QtService;

public class QtAndroidService extends QtService
{
    private static final String TAG = "qt_service";

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Creating Service");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "Destroying Service");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        int ret = super.onStartCommand(intent, flags, startId);
        Log.i(TAG, "Service started");
        return ret;
    }

    public static void startQtAndroidService(Context context) {
        Log.i(TAG, "Starting Service");
        context.startService(new Intent(context, QtAndroidService.class));
    }

    public static void stopQtAndroidService(Context context) {
        Log.i(TAG, "Stopping Service");
        context.stopService(new Intent(context, QtAndroidService.class));
    }
}
