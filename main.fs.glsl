#version 440 core

out vec3 color;
in vec4 gl_FragCoord;

uniform dvec2 viewportDimensions;
uniform dvec4 bounds; // minI, maxI, minR, maxR

void main() {
	// [-2.0, 2.0]
    dvec2 c = vec2(
        gl_FragCoord.x * (bounds.w - bounds.z) / viewportDimensions.x + bounds.z,
        gl_FragCoord.y * (bounds.y - bounds.x) / viewportDimensions.y + bounds.x
    );
	
	
	
	// Mandelbrot formula
    dvec2 z = c;
    float iterations = 0.0;
    float maxIterations = 1000.0;
    const int imaxIterations = 1000;

    for(int i = 0; i < imaxIterations; i++) {
        double t = 2.0 * z.x * z.y + c.y;
        z.x = z.x * z.x - z.y * z.y + c.x;
        z.y = t;

        if(z.x * z.x + z.y * z.y > 4.0) {
            break;
        }

        iterations += 1.0;
    }



    if(iterations == maxIterations) {
        color = vec3(1.0, 1.0, 1.0);
    }
    else {
        color = vec3(0.0, 0.0, 0.0);
    }
}
