xof 0303txt 0032
template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template IndexedColor {
 <1630b820-7842-11cf-8f52-0040333594a3>
 DWORD index;
 ColorRGBA indexColor;
}

template MeshVertexColors {
 <1630b821-7842-11cf-8f52-0040333594a3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}


Mesh {
 8;
 -1.000000;-1.000000;-1.000000;,
 -1.000000;1.000000;-1.000000;,
 1.000000;1.000000;-1.000000;,
 1.000000;-1.000000;-1.000000;,
 -1.000000;-1.000000;1.000000;,
 1.000000;-1.000000;1.000000;,
 1.000000;1.000000;1.000000;,
 -1.000000;1.000000;1.000000;;
 12;
 3;0,1,2;,
 3;2,3,0;,
 3;4,5,6;,
 3;6,7,4;,
 3;0,3,5;,
 3;5,4,0;,
 3;3,2,6;,
 3;6,5,3;,
 3;2,1,7;,
 3;7,6,2;,
 3;1,0,4;,
 3;4,7,1;;

 MeshVertexColors {
  8;
  0;0.000000;0.000000;1.000000;0.000000;;,
  1;0.000000;0.000000;1.000000;0.000000;;,
  2;0.000000;0.000000;1.000000;0.000000;;,
  3;0.000000;0.000000;1.000000;0.000000;;,
  4;0.000000;0.000000;1.000000;0.000000;;,
  5;0.000000;0.000000;1.000000;0.000000;;,
  6;0.000000;0.000000;1.000000;0.000000;;,
  7;0.000000;0.000000;1.000000;0.000000;;;
 }

 MeshMaterialList {
  2;
  12;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;

  Material {
   0.000000;0.000000;0.000000;0.000000;;
   38.400000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }

  Material {
   0.000000;0.000000;0.000000;0.000000;;
   38.400000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
}