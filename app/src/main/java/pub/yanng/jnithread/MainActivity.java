package pub.yanng.jnithread;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.util.UUID;

public class MainActivity extends Activity implements View.OnClickListener{

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private Button button1;
    private TextView tvLog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        nativeInit();

        button1 = (Button)findViewById(R.id.button1);
        button1.setOnClickListener(this);

        tvLog = (TextView)findViewById(R.id.tv_log);

    }

    public native void posixThread();

    public native void nativeInit();

    public native void nativeFree();

    @Override
    public void onClick(View view) {
        if (view == button1){
            posixThread();
            Log.i("pub.yanng.threqd","main thread");
        }
    }


    public void onNativeResult(final String log){
        Log.i("pub.yanng.thread","current thread:"+
                Thread.currentThread().getId() + ",log:"+log);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                //显示C回调的字符串
                tvLog.setText(tvLog.getText().toString() + "\n" + log);
            }
        });

    }

    @Override
    protected void onDestroy() {

        nativeFree();

        super.onDestroy();
    }
}
