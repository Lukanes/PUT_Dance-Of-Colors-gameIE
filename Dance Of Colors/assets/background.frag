uniform float time;
uniform int phase;
uniform vec2 resolution;
uniform vec4 platforms[20];
uniform int numPlatforms;

// Generator losowości
float hash(float x) { return fract(sin(x) * 43758.5453123); }
float hash2(vec2 p) { return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453); }

// Gładki szum Perlina (64-bitowy standard płynności)
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(hash2(i), hash2(i + vec2(1.0, 0.0)), u.x),
               mix(hash2(i + vec2(0.0, 1.0)), hash2(i + vec2(1.0, 1.0)), u.x), u.y);
}

void main() {
    // Natywna rozdzielczość, brak kanciastych powiększeń
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    uv.y = 1.0 - uv.y;

    // Bezwzględne, jasnoszare tło dla czytelności (178/178/178)
    vec3 bgColor = vec3(0.7, 0.7, 0.7); 
    vec3 finalColor = bgColor;

    if (phase == 0) {
        // --- OGIEŃ (Miękkie iskry i szum cieplny) ---
        vec2 uvHeat = uv;
        uvHeat.x += sin(uv.y * 5.0 + time * 2.0) * 0.01; // Falowanie gorącego powietrza
        
        float embers = 0.0;
        // Nakładanie kilku warstw miękkich iskier
        for(float i = 0.0; i < 4.0; i++) {
            vec2 p = uvHeat * vec2(8.0 + i * 2.0, 4.0 + i * 2.0);
            p.y += time * (0.8 + i * 0.4);
            float n = noise(p);
            // smoothstep wygładza krawędzie iskier
            embers += smoothstep(0.8, 1.0, n) * (0.5 + 0.5 * sin(time * 2.0 + i));
        }
        
        vec3 fireColor = vec3(0.9, 0.4, 0.1);
        finalColor = mix(bgColor, fireColor, clamp(embers, 0.0, 1.0));
        
        // Dolna, miękka łuna gorąca
        float glow = smoothstep(0.6, 1.0, uv.y) * 0.15;
        finalColor = mix(finalColor, vec3(1.0, 0.6, 0.1), glow);
    }
    else if (phase == 1) {
        // --- WODA (Gładki deszcz i organiczne fale) ---
        float dropIntensity = 0.0;
        float rippleIntensity = 0.0;
        
        float columns = 120.0;
        float id = floor(uv.x * columns);
        float colCenter = (id + 0.5) / columns;
        float speed = 1.5 + hash(id) * 2.0;

        float actualX = colCenter * resolution.x;
        float splashY = 1.0;
        
        // Obliczanie kolizji
        for(int i = 0; i < 20; i++) {
            if (i >= numPlatforms) break;
            if (actualX >= platforms[i].x && actualX <= platforms[i].x + platforms[i].z) {
                float normY = platforms[i].y / resolution.y;
                if (normY < splashY) splashY = normY;
            }
        }

        float dropY = fract(time * speed + hash(id)) * 1.2;
        
        // Rysowanie gładkich igieł deszczu
        if (dropY < splashY && uv.y < dropY && uv.y > dropY - 0.15) {
            float fade = smoothstep(dropY - 0.15, dropY, uv.y);
            float thickness = smoothstep(0.3, 0.0, abs(uv.x - colCenter) * columns);
            dropIntensity += pow(fade, 2.0) * thickness;
        }

        // Rysowanie miękkich pierścieni (ripple) na platformach
        if (dropY >= splashY && dropY < splashY + 0.15) {
            float t = (dropY - splashY) / 0.15; 
            vec2 diff = uv - vec2(colCenter, splashY);
            float dist = sqrt(diff.x * diff.x + diff.y * diff.y * 20.0); // Elipsa
            float maxR = t * 0.05;
            // Podwójny smoothstep daje perfekcyjnie miękką obręcz
            float ring = smoothstep(maxR + 0.005, maxR, dist) * smoothstep(maxR - 0.005, maxR, dist);
            rippleIntensity += ring * (1.0 - t);
        }
        
        finalColor = mix(bgColor, vec3(0.2, 0.3, 0.8), clamp(dropIntensity + rippleIntensity, 0.0, 1.0));
    }
    else if (phase == 2) {
        // --- ZIEMIA (Gładka burza piaskowa i opływowe kamienie) ---
        // Szum Perlina dla mgły piaskowej
        float sand = noise(uv * vec2(30.0, 10.0) + vec2(time * 0.8, time * 0.2));
        finalColor = mix(bgColor, vec3(0.6, 0.5, 0.4), sand * 0.15);

        // Kamienie w tle
        vec2 rGrid = floor(uv * vec2(15.0, 10.0));
        float rMove = time * (1.5 + hash(rGrid.x) * 2.0);
        vec2 rScroll = vec2(uv.x * 15.0, uv.y * 10.0 - rMove);
        vec2 rId = floor(rScroll);
        
        if (hash2(vec2(rId.x, rId.y)) > 0.9) {
            vec2 rLuv = fract(rScroll);
            float d = length(rLuv - 0.5);
            // Krawędzie kamienia "zmiękczone" i połączone z szumem
            float rockShape = smoothstep(0.25, 0.15, d + noise(rLuv * 5.0) * 0.1);
            finalColor = mix(finalColor, vec3(0.3, 0.25, 0.2), rockShape);
        }
    }
    else if (phase == 3) {
        // --- POWIETRZE (Gładkie, wektorowe smugi wiatru) ---
        float wind = 0.0;
        float wavyY = uv.y + sin(uv.x * 4.0 - time * 2.0) * 0.05;
        
        // Nakładanie kilku linii wiatru z różną prędkością
        for(float i = 0.0; i < 3.0; i++) {
            float rows = 15.0 + i * 5.0;
            float idY = floor(wavyY * rows);
            float speedA = 1.0 + hash(idY + i) * 2.0;
            float cycleA = fract(uv.x * 0.5 - time * speedA + hash(idY));
            float distY = abs(wavyY - (idY + 0.5) / rows) * rows;
            
            if (hash(idY + i * 10.0) > 0.6) {
                // Idealne wygładzenie ogona wiatru i samego brzegu linii
                float trail = smoothstep(0.5, 0.1, cycleA) * smoothstep(0.0, 0.1, cycleA);
                float thickness = smoothstep(0.4, 0.0, distY);
                wind += trail * thickness;
            }
        }
        finalColor = mix(bgColor, vec3(0.4, 0.8, 0.7), clamp(wind, 0.0, 1.0));
    }

    gl_FragColor = vec4(finalColor, 1.0);
}