using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine.Rendering;

public class RenderingPlugin : MonoBehaviour
{
#if UNITY_IPHONE && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
	[DllImport("RenderingPlugin")]
#endif
	private static extern IntPtr GetRenderEventFunc();

	void Start()
	{

	}

	public Material testMat;

	        // native render
        Dictionary<int, Mesh> simpleMeshes = new Dictionary<int, Mesh>();

        void CreateSimpleMesh(ref Mesh mesh)
        {
            Mesh newMesh = new Mesh();
            Vector3[] vectices = new Vector3[3];
            vectices[0] = new Vector3(0.0f, 0.0f, 0.0f);
            vectices[1] = new Vector3(0.1f, 0.0f, 0.0f);
            vectices[2] = new Vector3(0.1f, 0.0f, 0.1f);

            Vector2[] uv = new Vector2[3];
            uv[0] = new Vector2(0.0f, 0.0f);
            uv[1] = new Vector2(0.1f, 0.0f);
            uv[2] = new Vector2(0.1f, 0.1f);

            Color[] colors = new Color[3];
            colors[0] = new Color(1.0f, 0.0f, 0.0f);
            colors[1] = colors[0];
            colors[2] = colors[0];

            int[] triangles = { 0, 1, 2 };
            newMesh.vertices = vectices;
            newMesh.colors = colors;
            newMesh.uv = uv;
            newMesh.triangles = triangles;
            newMesh.RecalculateNormals();
            newMesh.RecalculateBounds();
            newMesh.UploadMeshData(false);

            mesh = newMesh;
        }

	void OnPostRender()
	{
        //testMat.SetPass(0);

        if (!simpleMeshes.ContainsKey(252))
        {
            Mesh newmesh = null;
            CreateSimpleMesh(ref newmesh);
            simpleMeshes.Add(252, newmesh);
        }

        CommandBuffer cb = new CommandBuffer();
        cb.DrawMesh(simpleMeshes[252], Matrix4x4.identity, testMat, 0, 0);
        cb.IssuePluginEvent(GetRenderEventFunc(), 1);

        Graphics.ExecuteCommandBuffer(cb);

        //Graphics.DrawMeshNow(simpleMeshes[252], Matrix4x4.identity);
		//GL.IssuePluginEvent(GetRenderEventFunc(), 1);
	}
}
