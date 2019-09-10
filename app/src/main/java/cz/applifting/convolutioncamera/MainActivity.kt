package cz.applifting.convolutioncamera

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.os.Bundle
import android.provider.MediaStore
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.LifecycleOwner
import kotlinx.android.synthetic.main.activity_main.*
import java.nio.ByteBuffer
import kotlin.system.measureTimeMillis




const val PICK_IMAGE = 10

class MainActivity : AppCompatActivity(), LifecycleOwner {

    lateinit var bitmap: Bitmap

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val data = byteArrayOf(1, 2, 3, 4, 5)

        val input = ByteBuffer.allocateDirect(data.size)
        input.put(data, 0, data.size)

        val res1 = jniCompute(input)

        for (i in 0 until input.capacity()) {
            Log.d("asdf", input[i].toString())
        }


        btn_pick.setOnClickListener { openPicker() }

        btn_rotate.setOnClickListener { convertCurrent(0) }

        btn_filter.setOnClickListener {
            convertCurrent(1)
            swtch_native.isChecked = true
        }
    }

    public override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)

        if (requestCode == PICK_IMAGE && resultCode == Activity.RESULT_OK && data != null) {
            val imageUri = data.data
            bitmap = MediaStore.Images.Media.getBitmap(this.contentResolver, imageUri)
            iv_image.setImageBitmap(bitmap)
        }
    }

    private fun openPicker() {
        val intent = Intent()
        intent.type = "image/*"
        intent.action = Intent.ACTION_GET_CONTENT
        startActivityForResult(Intent.createChooser(intent, "Select Picture"), PICK_IMAGE)
    }

    private fun convertCurrent(operation: Int) {
        val time = measureTimeMillis {
            bitmap = if (swtch_native.isChecked) {
                jniConvertBitmap(bitmap, operation)
            } else {
                convertBitmap(bitmap, operation)
            }
        }

        tv_time.text = time.toString()
        iv_image.setImageBitmap(bitmap)
    }


    fun myMethodFromJni(s: String): Int {
        Log.d("asdf", "Got here!!!")
        return 0
    }

    external fun jniGetString(): String

    external fun jniCompute(input: ByteBuffer): Int

    external fun jniConvertBitmap(bitmap: Bitmap, operation: Int): Bitmap

    companion object {

        val context: Context? = null
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
