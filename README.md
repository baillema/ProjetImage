# ProjetImage
Projet de simulation d'image

# Step of the project

# Step 1 : Heighfield
- Generate the heighfield, procedural perlin noise (http://mrl.nyu.edu/~perlin/)
- Shader given

# Step 2 : Normal Map
- Generate the normal map associated with the heighfield
- Shader given

# Step 3 : Render
- Render a tessalated grid (need VAO)
- Displace vertices depending on the heighfield (displacement mapping?)
- Use the normal map
- Color, texture, particular rendering application (cel shading?)
- Export other buffers (normal, dephtmap...)

# Step 4 : Post-process
- Apply effects (fog, sharpening, filtering, motion blur...)

# Step 5 : Shadows
- Generate a shadow map
- Send the shadow map in the rendering pass (step 3)
