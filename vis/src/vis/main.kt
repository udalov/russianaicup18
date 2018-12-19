package vis

import vis.test.createTestGameStates
import java.io.File
import kotlin.concurrent.thread

sealed class VisualizerMode {
    class FromFile(val file: File) : VisualizerMode()
    object FromLiveData : VisualizerMode()
    object Test : VisualizerMode()
}

typealias VisualizeUpdateCallback = (StepData) -> Unit

/**
 * In case `mode` is `VisualizerMode.FromLiveData`, returns a callback to add another step, otherwise returns null
 */
fun run(mode: VisualizerMode): VisualizeUpdateCallback? {
    // TODO?
    System.setProperty("awt.useSystemAAFontSettings", "on")
    System.setProperty("swing.aatext", "true")

    if (mode is VisualizerMode.FromFile) {
        val file = mode.file
        if (!file.exists() || file.isDirectory) {
            throw AssertionError("File does not exist: $file")
        }
    }

    var frame: VisualizerFrame? = null
    val data = GameData()

    val updateAction: () -> Unit = {
        when (mode) {
            is VisualizerMode.FromFile -> {
                data.clear()
                thread(name = "readStates") {
                    readStepsFromFile(mode.file, frame!!::addStepData)
                }
            }
            is VisualizerMode.FromLiveData -> {
                // do nothing
            }
            is VisualizerMode.Test -> {
                data.clear()
                createTestGameStates(frame!!::addStepData)
            }
        }
    }
    frame = VisualizerFrame(VisualState(), data, updateAction)

    if (mode is VisualizerMode.FromLiveData) {
        return { stepData -> frame.addStepData(data.stepDatas.size, stepData) }
    }

    return null
}

fun main(args: Array<String>) {
    val file = args.getOrNull(0)?.let(::File)
    if (file != null) {
        run(VisualizerMode.FromFile(file))
    } else {
        run(VisualizerMode.Test)
    }
}
