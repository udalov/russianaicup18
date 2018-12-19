package vis

import java.util.concurrent.ConcurrentHashMap

class GameData(
    val stepDatas: MutableMap<Int, StepData> = ConcurrentHashMap(),
    @Volatile var currentStepNumber: Int? = null
) {
    val currentStepData: StepData?
        get() = currentStepNumber?.let(stepDatas::get)

    fun clear() {
        currentStepNumber = null
        stepDatas.clear()
    }
}

internal fun GameData.nextState(): Boolean {
    val next = (currentStepNumber ?: 0) + 1
    if (next <= stepDatas.size - 1) {
        currentStepNumber = next
        return true
    }
    return false
}

internal fun GameData.previousState() {
    currentStepNumber = ((currentStepNumber ?: 0) - 1).coerceAtLeast(0)
}
