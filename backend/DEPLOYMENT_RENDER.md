# Backend Deployment Guide - Free Tier

This guide shows how to deploy the C++ Cloud Storage Server to **Render.com** for **FREE**.

## Prerequisites
- GitHub account (already have)
- This repository pushed to GitHub (already done ✅)
- Docker image working locally (tested ✅)

## Free Deployment Options

| Provider | Free Tier | Setup Time | Auto-Deploy |
|----------|-----------|-----------|-------------|
| **Render** (Recommended) | 512MB RAM, shared CPU | 5 min | Yes |
| Railway | $5/month credit | 5 min | Yes |
| Heroku | ❌ No free tier | N/A | N/A |

---

## Deploy to Render.com (FREE) - Step by Step

### Step 1: Create Render Account
1. Go to **https://render.com**
2. Click **Sign up**
3. Choose **Sign up with GitHub** (easiest)
4. Authorize Render to access your GitHub
5. Done! ✅

### Step 2: Create a Web Service
1. In Render dashboard, click **New +** button (top right)
2. Select **Web Service**
3. Select **Build and deploy from a Git repository**
4. Click **Connect Account** (if needed) to authorize GitHub
5. In the list, find and select: **Dhruvrawat04/Cloud-storageSimulator**
6. Click **Connect**

### Step 3: Configure Deployment
Fill in these fields:

| Field | Value | Notes |
|-------|-------|-------|
| **Name** | `cloud-sim-server` | Used in the URL |
| **Branch** | `main` | Deploys from this branch |
| **Build Command** | (leave empty) | Dockerfile detected automatically |
| **Start Command** | (leave empty) | CMD in Dockerfile will run |
| **Environment** | Docker | Already detected |
| **Instance Type** | **Free** | ⚠️ Important: Select this to avoid charges |
| **Region** | Oregon (or closest) | Choose geographically close region |

### Step 4: Add Environment Variables
1. Scroll down to **Environment** section
2. Click **Add Environment Variable**
3. Add:
   - **Key**: `PORT`
   - **Value**: `3001`
4. Click **Save**

### Step 5: Deploy
1. Click **Create Web Service** button
2. Watch the deployment logs (takes 2-5 minutes)
3. Once complete, you'll see: **"Your service is live at: https://cloud-sim-server-xxxxx.onrender.com"**

### Step 6: Verify Deployment
Once the green checkmark appears and logs show "Cloud Storage Server starting...", test the API:

```bash
# Test endpoint 1: Get files
curl https://cloud-sim-server-xxxxx.onrender.com/api/files

# Test endpoint 2: Check if server is responding
curl -I https://cloud-sim-server-xxxxx.onrender.com/api/files
```

Or use PowerShell:
```powershell
Invoke-WebRequest -Uri "https://cloud-sim-server-xxxxx.onrender.com/api/files"
```

---

## Important: Render Free Tier Behavior

⚠️ **App will sleep after 15 minutes of inactivity**
- First request after sleep = takes 30-60 seconds to start (cold start)
- After warm-up, responses are fast
- This is normal for free tier

✅ **To avoid sleep** (optional, paid):
- Upgrade to **Standard** tier ($7/month) for always-on
- OR keep free and accept cold starts

---

## Frontend Connection

Once backend is deployed, update your frontend with the API URL:

### For Vercel deployment:
1. Go to your Vercel project
2. Settings → Environment Variables
3. Add: `VITE_API_URL=https://cloud-sim-server-xxxxx.onrender.com`
4. Redeploy (Vercel will automatically rebuild)

### For local testing:
In your frontend code, update the API base URL to:
```typescript
const API_BASE = import.meta.env.VITE_API_URL || 'https://cloud-sim-server-xxxxx.onrender.com';
```

---

## Troubleshooting

### App won't build
1. Check **Logs** tab in Render dashboard
2. Look for CMake or build errors
3. Verify `backend/Dockerfile` exists and is correct
4. Common issue: Missing dependencies in Dockerfile

### API returns 500 error
1. Click **Logs** in Render dashboard
2. Look for server error messages
3. Common issue: PORT env var not set (should be 3001)

### Cold start takes too long
- This is normal for free tier
- App sleeps after 15 min inactivity, needs warm-up time
- Upgrade to paid tier if you need instant responses

### Can't find deployment URL
- In Render dashboard, click the service name
- Top of page shows the URL: `https://cloud-sim-server-xxxxx.onrender.com`

---

## Next Steps

1. ✅ Deploy backend to Render (this guide)
2. 📝 Deploy frontend to Vercel
3. 🔗 Connect them via VITE_API_URL
4. 🧪 Test end-to-end
5. 📊 Monitor logs and performance

---

## Auto-Deploy on Push

Once connected, **every push to `main` branch will automatically redeploy**:
```bash
git add .
git commit -m "Update backend"
git push origin main
# Render automatically builds and deploys!
```

---

## Questions?
- Render Docs: https://render.com/docs
- Cloud Storage Simulator Repo: https://github.com/Dhruvrawat04/Cloud-storageSimulator
