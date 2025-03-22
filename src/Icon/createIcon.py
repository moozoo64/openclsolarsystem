#Created by grok3 with Think
#I need an icon.png file of the sun with several planets orbiting it.
#256x256 pixels for best quality
#PNG format with transparency
#Simple enough to be recognizable at smaller sizes
#Follow the Freedesktop Icon Theme Specification guidelines
#Please create a python script to create this icon file
import math
from PIL import Image, ImageDraw

# Create a new image with a transparent background
img = Image.new('RGBA', (256, 256), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)

# Define the center of the image
center_x, center_y = 128, 128

# Draw the orbital paths (faint gray circles)
orbital_radii = [70, 90, 110]
for radius in orbital_radii:
    bbox = (center_x - radius, center_y - radius, center_x + radius, center_y + radius)
    draw.ellipse(bbox, outline='#CCCCCC')

# Draw the sun (yellow circle at the center)
sun_radius = 50
sun_bbox = (center_x - sun_radius, center_y - sun_radius, center_x + sun_radius, center_y + sun_radius)
draw.ellipse(sun_bbox, fill='#FFD700')

# Define the planets: distance from center, angle in degrees, and color
planets = [
    {'distance': 70, 'angle': 0, 'color': '#0000FF'},    # Blue planet
    {'distance': 90, 'angle': 120, 'color': '#00FF00'},  # Green planet
    {'distance': 110, 'angle': 240, 'color': '#FF0000'}, # Red planet
]

# Draw the planets
planet_radius = 10
for planet in planets:
    # Convert angle to radians for trigonometric functions
    theta = math.radians(planet['angle'])
    # Calculate the center position of the planet
    x = center_x + planet['distance'] * math.cos(theta)
    y = center_y + planet['distance'] * math.sin(theta)
    # Define the bounding box for the planet
    planet_bbox = (x - planet_radius, y - planet_radius, x + planet_radius, y + planet_radius)
    draw.ellipse(planet_bbox, fill=planet['color'])

# Save the image as a PNG file
img.save('icon.png', 'PNG')