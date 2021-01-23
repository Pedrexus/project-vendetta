# Game Application Layer

interface between game and operating system/device.

## Remarks

- application layer is a global singleton object and is referred to throughout your code through a pointer


### Windows Specifics

- `WinMain`: Windows entrypoint - replaces `int main()`
	- sets up calls for DirectX
	- runs the initialization sequence
	- enters the main loop
	- runs any shutdown code after the main loop exits