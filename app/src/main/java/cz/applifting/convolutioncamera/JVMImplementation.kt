package cz.applifting.convolutioncamera

import android.graphics.Bitmap
import android.util.Log
import java.nio.IntBuffer
import kotlin.system.measureTimeMillis


/**
 * Created by Petr Kubes <petr.kubes@applifting.cz> on 08, September, 2019
 */

fun IntArray.bytes(): ByteArray {
    return this.foldIndexed(ByteArray(this.size)) { i, a, v -> a.apply { set(i, v.toByte()) } }
}

fun convertBitmap(bitmap: Bitmap, operation: Int): Bitmap {

    val newBitmapPixels = IntBuffer.allocate(bitmap.height * bitmap.width)
    val oldBitmapPixels = IntBuffer.allocate(bitmap.height * bitmap.width)
    bitmap.copyPixelsToBuffer(oldBitmapPixels)

    val width = bitmap.width
    val height = bitmap.height

    bitmap.recycle()

    var whereToPut = 0
    val time = measureTimeMillis {
        for (x in width - 1 downTo 0) {
            for (y in 0 until height) {
                val pixel = oldBitmapPixels[width * y + x]
                newBitmapPixels.put(whereToPut++, pixel)
            }
        }
    }

    Log.d("asdf", time.toString())


    newBitmapPixels.rewind()
    val result = Bitmap.createBitmap(height, width, Bitmap.Config.ARGB_8888)
    result.copyPixelsFromBuffer(newBitmapPixels)
    return result
}
