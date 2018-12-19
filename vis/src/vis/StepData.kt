package vis

import java.awt.Graphics2D
import java.util.function.Consumer

class StepData(
    val header: List<String>,
    val text: List<String>,
    val drawWorld: Consumer<Graphics2D>,
    val drawExtras: Consumer<Graphics2D>
)
