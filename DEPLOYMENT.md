# Cloud Storage Simulator - Deployment Guide

Complete guide to deploy this project for **FREE** using Render (backend) and Vercel (frontend).

## Quick Overview

| Component | Technology | Hosting | Cost | Status |
|-----------|-----------|---------|------|--------|
| **Frontend** | React + Vite | Vercel | FREE | Ready ✅ |
| **Backend** | C++ Server | Render | FREE | Ready ✅ |
| **Database** | N/A (File-based) | Included | FREE | N/A |

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│              User Browser                               │
│         (Visits Vercel Frontend URL)                    │
└────────────────────┬────────────────────────────────────┘
                     │
                     │ HTTPS Requests
                     │
        ┌────────────▼────────────┐
        │  FRONTEND (Vercel)      │
        │  cloud-app.vercel.app   │
        │  - React + Vite         │
        │  - Static Site          │
        └────────────┬────────────┘
                     │
                     │ API Calls
                     │ (VITE_API_URL env var)
                     │
        ┌────────────▼────────────────────┐
        │  BACKEND (Render)               │
        │  cloud-sim-server.onrender.com  │
        │  - C++ HTTP Server              │
        │  - File Operations API          │
        │  - Process Scheduler            │
        └─────────────────────────────────┘
```

## Deployment Steps

### 1️⃣ Deploy Backend to Render (FREE)

**Full guide**: [backend/DEPLOYMENT_RENDER.md](backend/DEPLOYMENT_RENDER.md)

Quick steps:
1. Go to https://render.com → Sign up with GitHub
2. Create new **Web Service**
3. Select repo: `Dhruvrawat04/Cloud-storageSimulator`
4. **Name**: `cloud-sim-server`
5. **Instance Type**: Free
6. **Add env var**: `PORT=3001`
7. **Deploy**

**Result**: Backend URL like `https://cloud-sim-server-xxxxx.onrender.com`

---

### 2️⃣ Deploy Frontend to Vercel (FREE)

**Quick steps:**
1. Go to https://vercel.com → Sign up with GitHub
2. Click **Import Project**
3. Select repo: `Dhruvrawat04/Cloud-storageSimulator`
4. **Framework**: Auto-detect (Vite will be selected)
5. **Root Directory**: Leave as `/` (or auto-detected)
6. **Environment Variables** → Add:
   - **Name**: `VITE_API_URL`
   - **Value**: `https://cloud-sim-server-xxxxx.onrender.com` (your Render backend URL)
7. Click **Deploy**

**Result**: Frontend URL like `https://cloud-storage-simulator.vercel.app`

---

### 3️⃣ Connect Frontend & Backend

After both are deployed:

1. Test the connection by visiting your frontend URL
2. Try uploading/downloading files
3. Check browser console for API errors
4. If CORS errors appear, ensure backend allows requests

---

## Environment Variables

### Backend (Render)
```
PORT=3001
```

### Frontend (Vercel)
```
VITE_API_URL=https://cloud-sim-server-xxxxx.onrender.com
```

---

## Local Testing Before Deploy

### Test backend locally with Docker:
```bash
cd backend
docker build -t cloud-sim:local .
docker run --rm -p 3001:3001 cloud-sim:local
# In another terminal:
curl http://localhost:3001/api/files
```

### Test frontend locally:
```bash
npm install
npm run dev
# Visit http://localhost:5173
```

---

## URLs After Deployment

| Service | URL | Notes |
|---------|-----|-------|
| Frontend | `https://cloud-storage-simulator.vercel.app` | Replace with your actual Vercel URL |
| Backend | `https://cloud-sim-server-xxxxx.onrender.com` | Replace with your actual Render URL |

---

## File Structure

```
Cloud-storageSimulator/
├── backend/                    # C++ Backend
│   ├── Dockerfile             # ✅ Multi-stage build
│   ├── CMakeLists.txt
│   ├── main.cpp               # ✅ Reads PORT env var
│   ├── fly.toml              # Fly.io config (optional)
│   ├── render.yaml           # ✅ Render config
│   └── DEPLOYMENT_RENDER.md  # ✅ Deployment guide
│
├── src/                        # React Frontend
│   ├── App.tsx
│   ├── main.tsx
│   ├── vite-env.d.ts
│   └── components/
│
├── package.json              # ✅ Frontend dependencies
├── vite.config.ts           # ✅ Vite config
├── render.yaml              # ✅ Root Render config
└── README.md                # This file
```

---

## Deployment Checklist

- [ ] Backend files committed to GitHub
  - [ ] `backend/Dockerfile` ✅
  - [ ] `backend/main.cpp` (PORT env var) ✅
  - [ ] `render.yaml` ✅
- [ ] Frontend files committed to GitHub
  - [ ] `package.json` ✅
  - [ ] `vite.config.ts` ✅
  - [ ] `src/` components ✅
- [ ] Backend deployed to Render
  - [ ] Account created ✅
  - [ ] Web Service created ✅
  - [ ] URL received ✅
- [ ] Frontend deployed to Vercel
  - [ ] Account created ✅
  - [ ] Import project ✅
  - [ ] VITE_API_URL set to backend URL ✅
  - [ ] URL received ✅
- [ ] End-to-end testing
  - [ ] Frontend loads ✅
  - [ ] API calls work ✅
  - [ ] File upload/download works ✅

---

## Costs

| Component | Provider | Free Tier | Cost |
|-----------|----------|-----------|------|
| Frontend | Vercel | Included | $0/month |
| Backend | Render | 512MB RAM, shared CPU | $0/month |
| Domain | Provided | .vercel.app, .onrender.com | $0/month |
| **Total** | | | **$0/month** |

---

## Important Notes

### Render Free Tier
- ⚠️ App sleeps after 15 min of inactivity
- ⚠️ First request after sleep = 30-60 sec cold start
- ✅ Automatic HTTPS
- ✅ Auto-redeploy on git push
- 💡 Upgrade to $7/month Standard tier for always-on

### Vercel Free Tier
- ✅ Unlimited static sites
- ✅ Automatic HTTPS
- ✅ Global CDN
- ✅ Auto-redeploy on git push
- ✅ No cold starts for static sites

---

## Troubleshooting

**Frontend not connecting to backend?**
1. Check `VITE_API_URL` env var in Vercel
2. Verify Render backend is running
3. Check browser console for CORS errors
4. Try API directly: `curl https://backend-url/api/files`

**Backend won't build on Render?**
1. Check build logs in Render dashboard
2. Ensure `backend/Dockerfile` exists
3. Verify CMake can find dependencies

**App goes to sleep?**
- Normal for Render free tier
- First request takes 30-60 seconds to wake up
- Subsequent requests are fast

---

## Next Steps

1. Follow [backend/DEPLOYMENT_RENDER.md](backend/DEPLOYMENT_RENDER.md) to deploy backend
2. Follow steps above to deploy frontend to Vercel
3. Test the full application
4. Share the Vercel URL with others!

---

## Repository Links

- **GitHub**: https://github.com/Dhruvrawat04/Cloud-storageSimulator
- **Render Docs**: https://render.com/docs
- **Vercel Docs**: https://vercel.com/docs
- **Vite Docs**: https://vitejs.dev
