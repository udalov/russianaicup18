package vis

import java.awt.Point

class VisualState(
    val shift: Point = INITIAL_SHIFT,
    var zoom: Double = INITIAL_ZOOM,
    var lastWorldMouseLocation: Point? = null,
    var mousePressedInProgress: Boolean = false,
    var drawExtras: Boolean = false,
    @Volatile var autoPlay: Boolean = false,
    @Volatile var autoPlayTimeout: Double = INITIAL_AUTO_PLAY_TIMEOUT
)
