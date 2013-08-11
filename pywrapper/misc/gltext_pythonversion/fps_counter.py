
class FpsCounter:

    def __init__(self, update_interval_seconds = 0.5):
        """
        read self.fps for output
        """

        self.fps = 0.
        self.interval = update_interval_seconds

        self._counter = 0.
        self._age     = 0.
        self._last_output_age = 0.


    def tick(self, dt):

        self._age     += dt
        self._counter += 1.

        if self._age > self.interval:
            self.fps = self._counter / self._age
            self._age     = 0.
            self._counter = 0.

