'use strict';

const fs = require('fs');

function randomSigned() {
  return 2.0 * Math.random() - 1.0;
}

function toUnsigned(signed) {
  return 0.5 * signed + 0.5;
}

const townInfo = [
  {
    name: "Old Town",
    population: 452
  },
  {
    name: "New Town",
    population: 5234
  },
  {
    name: "Newer Town",
    population: 34245
  }
];

/**
 * Pretend this weather data is at the corner between 
 */
function getTownId(point) {
  if (point.z > Math.sin(point.x)) {
    return 0;
  } else if (point.x > point.z) {
    return 1;
  }

  return 2;
}

/**
 * Create a simple pressure gradient. The pressure is
 * higher on the left and lower on the right
 *
 * pressure is measured in atmospheres (atm)
 */
function getAirPressure(point) {
  const t = toUnsigned(point.x);
  const highPressure = 1.5;
  const lowPressure = 0.9;
  return (1.0 - t) * highPressure + t * lowPressure;
}

/**
 * create a made-up wind velocity field that is moving along the
 * x axis, but spreading in +z, -z and +y directions
 */
function getWindVelocity(point) {
  // Wind is moving from left to right
  const vx = 1.0;

  // Wind spreads out upwards
  const vy = 0.5 * toUnsigned(point.y);

  // Wind spreads out in both directions in z
  const vz = point.z * toUnsigned(point.x);

  // Wind is spreading out as it goes to the right
  return {
    x: vx,
    y: vy,
    z: vz
  };
}

/**
 * Make a temperature gradient warmest at the bottom and coldest at the top
 * with a little bit of noise thrown in.
 *
 * returns values around the range [20, 25] degrees celsius
 */
function getTemperature(point) {
  const t = toUnsigned(point.y);
  const highTemperature = 25.0;
  const lowTemperature = 20.0;
  const baseTemperature = (1.0 - t) * highTemperature + t * lowTemperature;
  const noise = 0.1 * Math.random();
  return baseTemperature + noise;
}

function makePoint() {
  const point = {
    x: randomSigned(),
    y: randomSigned(),
    z: randomSigned()
  };

  point.townId = getTownId(point);
  point.airPressure = getAirPressure(point);
  point.windVelocity = getWindVelocity(point);
  point.temperature = getTemperature(point);

  return point;
}

function makePoints(numberOfPoints) {
  const points = [];
  for (let i = 0; i < numberOfPoints; i++) {
    points.push(makePoint());
  }
  return points;
}

/**
 * Make the geometry buffer. This creates a .bin file
 * and returns metadata to insert into the gltf file
 */
function makeGeometryBuffer(fname, points) {
  const numberOfPoints = points.length;
  const min = {
    x: 1e10,
    y: 1e10,
    z: 1e10
  };
  const max = {
    x: -1e10,
    y: -1e10,
    z: -1e10
  };

  const positions = new Float32Array(3 * numberOfPoints);
  for (let i = 0; i < numberOfPoints; i++) {
    const point = points[i];
    min.x = Math.min(min.x, point.x);
    min.y = Math.min(min.y, point.y);
    min.z = Math.min(min.z, point.z);

    max.x = Math.max(max.x, point.x);
    max.y = Math.max(max.y, point.y);
    max.z = Math.max(max.z, point.z);

    positions[3 * i] = point.x;
    positions[3 * i + 1] = point.y;
    positions[3 * i + 2] = point.z;
  }

  const buffer = Buffer.from(positions.buffer);
  fs.writeFileSync(fname, buffer);

  return {
    numberOfPoints,
    accessor: {
      name: "Positions",
      bufferView: 0,
      byteOffset: 0,
      componentType: 5126, //float
      count: numberOfPoints,
      max: [max.x, max.y, max.z],
      min: [min.x, min.y, min.z],
      type: "VEC3"
    },
    bufferView: {
      name: "Positions",
      buffer: 0,
      byteLength: buffer.byteLength,
      byteOffset: 0
    },
    buffer: {
      name: "Geometry Buffer",
      byteLength: buffer.byteLength,
      uri: fname
    }
  }
}


function makePerVertexFeatures(points) {
  const numberOfPoints = points.length;
  const townId = new Float32Array(numberOfPoints);
  const temperature = new Float32Array(numberOfPoints);
  const airPressure = new Float32Array(numberOfPoints);
  const windVelocity = new Float32Array(numberOfPoints * 3);
  for (let i = 0; i < numberOfPoints; i++) {
    const point = points[i];
    townId[i] = point.townId;
    temperature[i] = point.temperature;
    airPressure[i] = point.airPressure;

    windVelocity[3 * i] = point.windVelocity.x;
    windVelocity[3 * i + 1] = point.windVelocity.y;
    windVelocity[3 * i + 2] = point.windVelocity.z;
  }

  return [
    {
      name: "Town Feature ID",
      bufferView: Buffer.from(townId.buffer)
    },
    {
      name: "Air Temperature",
      bufferView: Buffer.from(temperature.buffer)
    },
    {
      name: "Air Pressure",
      bufferView: Buffer.from(airPressure.buffer)
    },
    {
      name: "Wind Velocity",
      bufferView: Buffer.from(windVelocity.buffer)
    },
  ];
}

function makeTownFeatures() {
  const numberOfFeatures = townInfo.length;
  const offsets = new Uint32Array(numberOfFeatures + 1);
  const population = new Uint16Array(numberOfFeatures);
  let nameText = '';
  let currentOffset = 0;
  for (let i = 0; i < numberOfFeatures; i++) {
    const town = townInfo[i];
    const name = town.name;

    population[i] = town.population;

    offsets[i] = currentOffset;
    currentOffset += Buffer.byteLength(name, 'utf8');

    nameText += name;
  }
  offsets[numberOfFeatures] = currentOffset;

  return [
    {
      name: "Town Name",
      bufferView: Buffer.from(nameText, 'utf8')
    },
    {
      name: "Town Name Offsets",
      bufferView: Buffer.from(offsets.buffer)
    },
    {
      name: "Town Population",
      bufferView: Buffer.from(population.buffer)
    }
  ];
}

function makeMetadataBuffer(fname, points) {
  const bufferViewInfos = makePerVertexFeatures(points)
    .concat(makeTownFeatures());

  const bufferViews = [];
  const stats = [];
  let offset = 0;
  for (const bufferViewInfo of bufferViewInfos) {
      const bufferView = bufferViewInfo.bufferView;
      const byteLength = bufferView.byteLength;
      stats.push({
          name: bufferViewInfo.name,
          buffer: 1,
          byteLength: byteLength,
          byteOffset: offset
      });
      offset += byteLength;

      bufferViews.push(bufferView);
  }

  const buffer = Buffer.concat(bufferViews);
  fs.writeFileSync(fname, buffer, 'utf8');

  return {
    bufferViewIndices: {
      townIds: 1,
      airTemperature: 2,
      airPressure: 3,
      windVelocity: 4,
      townName: 5,
      townNameOffsets: 6,
      townPopulation: 7
    },
    townIdAccessor: {
      name: "Town Feature ID",
      bufferView: 1,
      byteOffset: 0,
      componentType: 5126, //float
      count: points.length,
      type: "SCALAR"
    },
    bufferViews: stats,
    buffer: {
      name: "Metadata Buffer",
      byteLength: buffer.byteLength,
      uri: fname
    }
  };
}

function main() {
  const points = makePoints(1000);
  const geometryInfo = makeGeometryBuffer('weather.bin', points);
  const metadataInfo = makeMetadataBuffer('weather-metadata.bin', points);
  const numberOfPoints = points.length;
  const numberOfTowns = townInfo.length;
  const gltf = {
    asset: {
      version: "2.0"
    },
    extensionsUsed: [
      "EXT_feature_metadata"
    ],
    extensions: {
      EXT_feature_metadata: {
        extras: {
          draftVersion: "0.0.0"
        },
        classes: {
          weather: {
            name: "Weather Data",
            properties: {
              airTemperature: {
                name: "Air Temperature",
                type: "FLOAT32"
              },
              airPressure: {
                name: "Air Pressure",
                type: "FLOAT32"
              },
              windVelocity: {
                name: "Wind Velocity",
                type: "ARRAY",
                componentType: "FLOAT32",
                componentCount: 3
              },
              semantic: {
                name: "Semantic",
                type: "STRING",
                optional: true,
                default: "WEATHER"
              }
            }
          },
          town: {
            name: "Town Data",
            properties: {
              name: {
                name: "Name",
                type: "STRING"
              },
              population: {
                name: "Population",
                type: "FLOAT32"
              },
              semantic: {
                name: "Semantic",
                type: "STRING",
                optional: true,
                default: "TOWN"
              }
            }
          }
        },
        featureTables: {
          weatherTable: {
            class: "weather",
            count: numberOfPoints,
            properties: {
              airTemperature: {
                bufferView: metadataInfo.bufferViewIndices.airTemperature
              },
              airPressure: {
                bufferView: metadataInfo.bufferViewIndices.airPressure
              },
              windVelocity: {
                bufferView: metadataInfo.bufferViewIndices.windVelocity
              }
            }
          },
          townTable: {
            class: "town",
            count: numberOfTowns,
            properties: {
              name: {
                bufferView: metadataInfo.bufferViewIndices.townName,
                offsetBufferViews: [metadataInfo.bufferViewIndices.townNameOffsets]
              },
              population: {
                bufferView: metadataInfo.bufferViewIndices.townPopulation
              }
            }
          }
        }
      }
    },
    scene: 0,
    scenes: [
      {
        name: "Scene",
        nodes: [0]
      }
    ],
    nodes: [
      {
        name: "Point Cloud",
        mesh: 0
      }
    ],
    meshes: [
      {
        name: "Point Cloud Mesh",
        primitives: [
          {
            attributes: {
              POSITION: 0,
              _FEATURE_ID_0: 1
            },
            mode: 0, //POINTS
            extensions: {
              EXT_feature_metadata: {
                featureIdAttributes: [
                  {
                    featureTable: "weatherTable",
                    featureIds: {
                      constant: 0,
                      vertexStride: 1
                    }
                  },
                  {
                    featureTable: "townTable",
                    featureIds: {
                      attribute: "_FEATURE_ID_0"
                    }
                  }
                ]
              }
            }
          }
        ]
      },
    ],
    accessors: [
      geometryInfo.accessor,
      metadataInfo.townIdAccessor
    ],
    bufferViews: [
      geometryInfo.bufferView,
      ...metadataInfo.bufferViews
    ],
    buffers : [
      geometryInfo.buffer,
      metadataInfo.buffer
    ]
  };
  fs.writeFileSync('weather.gltf', JSON.stringify(gltf, undefined, 2));
}

main();
