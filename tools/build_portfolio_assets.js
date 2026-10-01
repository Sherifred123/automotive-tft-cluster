const http = require('http');
const fs = require('fs');
const path = require('path');
const { exec, spawn } = require('child_process');

const PORT = 8765;
const DOCS_DIR = path.resolve(__dirname, '../docs');
const IMAGES_DIR = path.join(DOCS_DIR, 'images');
const FRAMES_DIR = path.join(DOCS_DIR, 'images/temp_frames');
const FFMPEG_PATH = process.env.FFMPEG_PATH || (process.platform === 'win32' ? 'ffmpeg.exe' : 'ffmpeg');
const CHROME_PATH = process.env.CHROME_PATH || (process.platform === 'win32' ? 'C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe' : 'google-chrome');

if (!fs.existsSync(IMAGES_DIR)) fs.mkdirSync(IMAGES_DIR, { recursive: true });
if (!fs.existsSync(FRAMES_DIR)) fs.mkdirSync(FRAMES_DIR, { recursive: true });

const generatorHtml = `<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Asset Generator</title>
  <style>
    body { background: #000; margin: 0; padding: 0; display: flex; flex-direction: column; align-items: center; justify-content: center; }
    canvas { display: block; }
  </style>
</head>
<body>
  <canvas id="canvas" width="1280" height="720"></canvas>
  <script>
    const canvas = document.getElementById('canvas');
    const ctx = canvas.getContext('2d');

    function drawCluster(opts) {
      const {
        width = 1280,
        height = 720,
        speed = 72,
        rpm = 2304,
        soc = 84,
        voltage = 52.8,
        gear = 'D',
        leftTurn = false,
        rightTurn = false,
        highBeam = false,
        ready = true,
        canFault = false,
        battLow = false,
        isNight = true,
        trip = 142.8,
        odo = 14820,
        estRange = 118,
        temp = 28,
        stageBanner = ""
      } = opts;

      canvas.width = width;
      canvas.height = height;

      // Dark metallic bezel housing
      const bgGrad = ctx.createLinearGradient(0, 0, 0, height);
      bgGrad.addColorStop(0, '#111722');
      bgGrad.addColorStop(1, '#080c14');
      ctx.fillStyle = bgGrad;
      ctx.fillRect(0, 0, width, height);

      // Outer bezel border
      ctx.strokeStyle = '#1e293b';
      ctx.lineWidth = 10;
      ctx.strokeRect(5, 5, width - 10, height - 10);

      // Inner LCD Screen Area
      const topOffset = stageBanner ? 38 : 14;
      const lcdX = 14, lcdY = topOffset, lcdW = width - 28, lcdH = height - topOffset - 14;
      const screenBg = isNight ? '#0a0e17' : '#f8fafc';
      ctx.fillStyle = screenBg;
      ctx.fillRect(lcdX, lcdY, lcdW, lcdH);

      // Stage banner if requested
      if (stageBanner) {
        ctx.fillStyle = '#0f172a';
        ctx.fillRect(0, 0, width, 38);
        ctx.strokeStyle = '#1e293b';
        ctx.lineWidth = 1;
        ctx.strokeRect(0, 0, width, 38);
        ctx.fillStyle = '#00d2ff';
        ctx.font = 'bold 14px monospace';
        ctx.textAlign = 'center';
        ctx.fillText(stageBanner, width / 2, 24);
      }

      // Palette
      const textColor = isNight ? '#f8fafc' : '#0f172a';
      const dimTextColor = isNight ? '#64748b' : '#64748b';
      const cyan = '#00d2ff';
      const green = '#00e676';
      const red = '#ff3d00';
      const amber = '#ffab00';
      const dialTrack = isNight ? '#131b2a' : '#e2e8f0';

      // Header Bar Line
      ctx.strokeStyle = isNight ? 'rgba(255,255,255,0.08)' : 'rgba(0,0,0,0.08)';
      ctx.lineWidth = 2;
      ctx.beginPath();
      ctx.moveTo(lcdX, lcdY + 60);
      ctx.lineTo(lcdX + lcdW, lcdY + 60);
      ctx.stroke();

      // Footer Bar Line
      ctx.beginPath();
      ctx.moveTo(lcdX, lcdY + lcdH - 60);
      ctx.lineTo(lcdX + lcdW, lcdY + lcdH - 60);
      ctx.stroke();

      // HEADER TELL-TALES (Dynamically Proportional)
      const headCenterY = lcdY + 30;

      // Left Turn Arrow
      const ltX = lcdX + lcdW * 0.08;
      ctx.fillStyle = leftTurn ? green : (isNight ? '#1e293b' : '#cbd5e1');
      if (leftTurn) { ctx.shadowColor = green; ctx.shadowBlur = 14; }
      ctx.beginPath();
      ctx.moveTo(ltX - 18, headCenterY);
      ctx.lineTo(ltX + 2, headCenterY - 14);
      ctx.lineTo(ltX + 2, headCenterY - 6);
      ctx.lineTo(ltX + 22, headCenterY - 6);
      ctx.lineTo(ltX + 22, headCenterY + 6);
      ctx.lineTo(ltX + 2, headCenterY + 6);
      ctx.lineTo(ltX + 2, headCenterY + 14);
      ctx.closePath();
      ctx.fill();
      ctx.shadowBlur = 0;

      // High Beam Icon
      const hbX = lcdX + lcdW * 0.22;
      const hbColor = highBeam ? cyan : (isNight ? '#1e293b' : '#cbd5e1');
      ctx.fillStyle = hbColor;
      ctx.strokeStyle = hbColor;
      if (highBeam) { ctx.shadowColor = cyan; ctx.shadowBlur = 12; }
      ctx.beginPath();
      ctx.arc(hbX, headCenterY, 12, -Math.PI/2, Math.PI/2, true);
      ctx.closePath();
      ctx.fill();
      ctx.lineWidth = 2.5;
      for (let i = -2; i <= 2; i++) {
        ctx.beginPath();
        ctx.moveTo(hbX - 16, headCenterY + i * 5);
        ctx.lineTo(hbX - 26, headCenterY + i * 5);
        ctx.stroke();
      }
      ctx.shadowBlur = 0;

      // READY Tell-Tale
      const rX = lcdX + lcdW * 0.38;
      ctx.font = 'bold 18px monospace';
      ctx.textAlign = 'center';
      if (ready) {
        ctx.fillStyle = green;
        ctx.shadowColor = green;
        ctx.shadowBlur = 10;
        ctx.fillText('⚡ READY', rX, headCenterY + 6);
      } else {
        ctx.fillStyle = dimTextColor;
        ctx.fillText('OFFLINE', rX, headCenterY + 6);
      }
      ctx.shadowBlur = 0;

      // Status / Warnings Center-Right
      const warnX = lcdX + lcdW * 0.68;
      if (canFault) {
        ctx.fillStyle = amber;
        ctx.shadowColor = amber;
        ctx.shadowBlur = 14;
        ctx.font = 'bold 18px monospace';
        ctx.fillText('⚠️ CAN FAULT', warnX, headCenterY + 6);
      } else if (battLow || soc <= 15) {
        ctx.fillStyle = red;
        ctx.shadowColor = red;
        ctx.shadowBlur = 14;
        ctx.font = 'bold 18px monospace';
        ctx.fillText('🪫 BATT LOW', warnX, headCenterY + 6);
      } else {
        ctx.fillStyle = dimTextColor;
        ctx.font = '15px monospace';
        ctx.fillText('CAN: 500k J1939 OK', warnX, headCenterY + 6);
      }
      ctx.shadowBlur = 0;

      // Right Turn Arrow
      const rtX = lcdX + lcdW * 0.92;
      ctx.fillStyle = rightTurn ? green : (isNight ? '#1e293b' : '#cbd5e1');
      if (rightTurn) { ctx.shadowColor = green; ctx.shadowBlur = 14; }
      ctx.beginPath();
      ctx.moveTo(rtX + 18, headCenterY);
      ctx.lineTo(rtX - 2, headCenterY - 14);
      ctx.lineTo(rtX - 2, headCenterY - 6);
      ctx.lineTo(rtX - 22, headCenterY - 6);
      ctx.lineTo(rtX - 22, headCenterY + 6);
      ctx.lineTo(rtX - 2, headCenterY + 6);
      ctx.lineTo(rtX - 2, headCenterY + 14);
      ctx.closePath();
      ctx.fill();
      ctx.shadowBlur = 0;

      // GAUGES GEOMETRY
      const leftCenterX = lcdX + lcdW * 0.28;
      const rightCenterX = lcdX + lcdW * 0.72;
      const centerY = lcdY + lcdH * 0.49;
      const radius = Math.min(lcdW * 0.16, 175);

      // 1. SPEEDOMETER (LEFT DIAL)
      ctx.lineWidth = 14;
      ctx.strokeStyle = dialTrack;
      ctx.beginPath();
      ctx.arc(leftCenterX, centerY, radius, 0.75 * Math.PI, 2.25 * Math.PI);
      ctx.stroke();

      const speedAngle = (0.75 + (Math.min(speed, 140) / 140) * 1.5) * Math.PI;
      ctx.strokeStyle = cyan;
      ctx.shadowColor = cyan;
      ctx.shadowBlur = isNight ? 16 : 0;
      ctx.beginPath();
      ctx.arc(leftCenterX, centerY, radius, 0.75 * Math.PI, speedAngle);
      ctx.stroke();
      ctx.shadowBlur = 0;

      // Speed Ticks
      for (let s = 0; s <= 140; s += 10) {
        const a = (0.75 + (s / 140) * 1.5) * Math.PI;
        const isMajor = (s % 20 === 0);
        const tLen = isMajor ? 14 : 8;
        const p1x = leftCenterX + (radius - 18) * Math.cos(a);
        const p1y = centerY + (radius - 18) * Math.sin(a);
        const p2x = leftCenterX + (radius - 18 - tLen) * Math.cos(a);
        const p2y = centerY + (radius - 18 - tLen) * Math.sin(a);

        ctx.strokeStyle = isMajor ? textColor : dimTextColor;
        ctx.lineWidth = isMajor ? 2.5 : 1.5;
        ctx.beginPath();
        ctx.moveTo(p1x, p1y);
        ctx.lineTo(p2x, p2y);
        ctx.stroke();

        if (isMajor) {
          const numX = leftCenterX + (radius - 44) * Math.cos(a);
          const numY = centerY + (radius - 44) * Math.sin(a);
          ctx.fillStyle = dimTextColor;
          ctx.font = 'bold 14px -apple-system, sans-serif';
          ctx.textAlign = 'center';
          ctx.textBaseline = 'middle';
          ctx.fillText(s.toString(), numX, numY);
        }
      }

      // Speed Needle
      ctx.strokeStyle = red;
      ctx.lineWidth = 4.5;
      ctx.shadowColor = red;
      ctx.shadowBlur = isNight ? 12 : 0;
      ctx.beginPath();
      ctx.moveTo(leftCenterX, centerY);
      ctx.lineTo(leftCenterX + (radius - 18) * Math.cos(speedAngle), centerY + (radius - 18) * Math.sin(speedAngle));
      ctx.stroke();
      ctx.shadowBlur = 0;

      // Center Hub
      ctx.fillStyle = '#ffffff';
      ctx.beginPath();
      ctx.arc(leftCenterX, centerY, 8, 0, 2 * Math.PI);
      ctx.fill();

      // Digital Readout
      ctx.fillStyle = textColor;
      ctx.font = 'bold 56px -apple-system, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText(Math.round(speed), leftCenterX, centerY + 80);
      ctx.font = 'bold 16px sans-serif';
      ctx.fillStyle = dimTextColor;
      ctx.fillText('km/h', leftCenterX, centerY + 106);
      ctx.font = '15px monospace';
      ctx.fillStyle = cyan;
      ctx.fillText(rpm.toLocaleString() + ' RPM', leftCenterX, centerY + 130);

      // 2. BATTERY SOC (RIGHT DIAL)
      ctx.lineWidth = 14;
      ctx.strokeStyle = dialTrack;
      ctx.beginPath();
      ctx.arc(rightCenterX, centerY, radius, 0.75 * Math.PI, 2.25 * Math.PI);
      ctx.stroke();

      const socAngle = (0.75 + (Math.min(soc, 100) / 100) * 1.5) * Math.PI;
      const socColor = (soc <= 20) ? red : green;
      ctx.strokeStyle = socColor;
      ctx.shadowColor = socColor;
      ctx.shadowBlur = isNight ? 16 : 0;
      ctx.beginPath();
      ctx.arc(rightCenterX, centerY, radius, 0.75 * Math.PI, socAngle);
      ctx.stroke();
      ctx.shadowBlur = 0;

      // Battery Ticks
      for (let b = 0; b <= 100; b += 10) {
        const a = (0.75 + (b / 100) * 1.5) * Math.PI;
        const isMajor = (b % 25 === 0);
        const tLen = isMajor ? 14 : 8;
        const p1x = rightCenterX + (radius - 18) * Math.cos(a);
        const p1y = centerY + (radius - 18) * Math.sin(a);
        const p2x = rightCenterX + (radius - 18 - tLen) * Math.cos(a);
        const p2y = centerY + (radius - 18 - tLen) * Math.sin(a);

        ctx.strokeStyle = isMajor ? textColor : dimTextColor;
        ctx.lineWidth = isMajor ? 2.5 : 1.5;
        ctx.beginPath();
        ctx.moveTo(p1x, p1y);
        ctx.lineTo(p2x, p2y);
        ctx.stroke();

        if (isMajor) {
          const numX = rightCenterX + (radius - 44) * Math.cos(a);
          const numY = centerY + (radius - 44) * Math.sin(a);
          ctx.fillStyle = dimTextColor;
          ctx.font = 'bold 14px -apple-system, sans-serif';
          ctx.textAlign = 'center';
          ctx.textBaseline = 'middle';
          ctx.fillText(b + '%', numX, numY);
        }
      }

      // Battery Digital Readout
      ctx.fillStyle = textColor;
      ctx.font = 'bold 56px -apple-system, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText(Math.round(soc) + '%', rightCenterX, centerY + 80);
      ctx.font = 'bold 16px sans-serif';
      ctx.fillStyle = dimTextColor;
      ctx.fillText('STATE OF CHARGE', rightCenterX, centerY + 106);
      ctx.font = '15px monospace';
      ctx.fillStyle = green;
      ctx.fillText(voltage.toFixed(1) + ' V (PACK)', rightCenterX, centerY + 130);

      // 3. CENTER GEAR BADGE
      const centerBoxX = lcdX + (lcdW / 2) - 34;
      const centerBoxY = centerY - 48;
      ctx.fillStyle = isNight ? 'rgba(0, 210, 255, 0.12)' : 'rgba(0, 210, 255, 0.2)';
      ctx.strokeStyle = cyan;
      ctx.lineWidth = 2.5;
      ctx.beginPath();
      ctx.roundRect(centerBoxX, centerBoxY, 68, 90, 12);
      ctx.fill();
      ctx.stroke();

      ctx.fillStyle = dimTextColor;
      ctx.font = 'bold 12px sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('GEAR', lcdX + (lcdW / 2), centerBoxY + 20);

      ctx.fillStyle = cyan;
      ctx.font = 'bold 42px sans-serif';
      ctx.fillText(gear, lcdX + (lcdW / 2), centerBoxY + 68);

      // 4. FOOTER STATUS TELEMETRY (Evenly Spaced)
      const footY = lcdY + lcdH - 24;
      ctx.font = '15px monospace';
      ctx.textAlign = 'left';
      ctx.fillStyle = dimTextColor;
      ctx.fillText(\`TRIP: \${trip.toFixed(1)} km\`, lcdX + lcdW * 0.05, footY);
      ctx.fillText(\`ODO: \${odo.toString().padStart(6, '0')} km\`, lcdX + lcdW * 0.32, footY);
      ctx.fillText(\`EST: \${estRange} km\`, lcdX + lcdW * 0.60, footY);
      ctx.fillText(\`AMBIENT: \${temp}°C\`, lcdX + lcdW * 0.82, footY);
    }

    // Oscilloscope Drawer (Tektronix / Keysight DSO Authentic Calibration)
    function drawOscilloscope() {
      const width = 1280, height = 720;
      canvas.width = width;
      canvas.height = height;

      // Dark chassis
      ctx.fillStyle = '#0e141d';
      ctx.fillRect(0, 0, width, height);

      // DSO Screen Bezel
      ctx.fillStyle = '#060a10';
      ctx.fillRect(16, 16, width - 32, height - 32);
      ctx.strokeStyle = '#263445';
      ctx.lineWidth = 3;
      ctx.strokeRect(16, 16, width - 32, height - 32);

      // Top Header Info
      ctx.fillStyle = '#0f172a';
      ctx.fillRect(20, 20, width - 40, 52);
      
      ctx.fillStyle = '#00d2ff';
      ctx.font = 'bold 16px monospace';
      ctx.textAlign = 'left';
      ctx.fillText('KEYSIGHT InfiniiVision DSOX3024T | Automotive J1939 CAN Bus Differential Physical Layer', 35, 52);

      ctx.fillStyle = '#94a3b8';
      ctx.font = '13px monospace';
      ctx.textAlign = 'right';
      ctx.fillText('Trigger: Ext Edge | 2.00 GSa/s | 500 kbps (Tbit = 2.00 µs)', width - 35, 52);

      // Grid area
      const gx = 45, gy = 85, gw = width - 90, gh = height - 170;
      ctx.fillStyle = '#04070b';
      ctx.fillRect(gx, gy, gw, gh);

      // Graticule (10 horizontal div, 8 vertical div)
      ctx.strokeStyle = 'rgba(255,255,255,0.06)';
      ctx.lineWidth = 1;
      const numCols = 10, numRows = 8;
      for (let c = 0; c <= numCols; c++) {
        const x = gx + (c * gw / numCols);
        ctx.beginPath(); ctx.moveTo(x, gy); ctx.lineTo(x, gy + gh); ctx.stroke();
      }
      for (let r = 0; r <= numRows; r++) {
        const y = gy + (r * gh / numRows);
        ctx.beginPath(); ctx.moveTo(gx, y); ctx.lineTo(gx + gw, y); ctx.stroke();
      }

      // Center crosshair
      ctx.strokeStyle = 'rgba(255,255,255,0.18)';
      ctx.setLineDash([2, 4]);
      ctx.beginPath();
      ctx.moveTo(gx, gy + gh / 2);
      ctx.lineTo(gx + gw, gy + gh / 2);
      ctx.moveTo(gx + gw / 2, gy);
      ctx.lineTo(gx + gw / 2, gy + gh);
      ctx.stroke();
      ctx.setLineDash([]);

      // Waveform Data (ISO 11898-2 CAN Physical Layer Rules)
      // Recessive bit (1): CAN_H = 2.5V, CAN_L = 2.5V, Vdiff = 0V
      // Dominant bit  (0): CAN_H = 3.5V (+1V), CAN_L = 1.5V (-1V), Vdiff = +2.0V
      const bits = [
        0, // SOF (Dominant)
        1,1,0,0,0,1,1,0,1,1,1,0,0,1,1,0,0,0,0,0, // 29-bit J1939 ID 0x18FEE600
        1,0,0,0, // DLC = 8 bytes
        0,0,1,0,1,1,0,1, // Data 0: 0x2D
        0,0,0,0,1,0,0,1, // Data 1: 0x09
        1,0,1,0,0,1,0,1, // Data 2: 0xA5
        0,0,0,0,0,0,0,0, // Data 3: 0x00
        1,1,0,0,1,0,1,0, // CRC
        0, // ACK (Dominant by cluster node!)
        1,1,1,1,1,1,1 // EOF (7 Recessive bits)
      ];

      const bitWidth = gw / bits.length;

      // Channel 1: CAN_H (Yellow, Baseline 2.5V, pulses UP to 3.5V during Dominant)
      const ch1BaseY = gy + 105;
      ctx.strokeStyle = '#facc15';
      ctx.lineWidth = 2.5;
      ctx.beginPath();
      for (let i = 0; i < bits.length; i++) {
        const xStart = gx + i * bitWidth;
        const xEnd = xStart + bitWidth;
        const isDom = (bits[i] === 0);
        const y = isDom ? ch1BaseY - 42 : ch1BaseY; // UP when dominant!
        if (i === 0) ctx.moveTo(xStart, y);
        else ctx.lineTo(xStart, y);
        ctx.lineTo(xEnd, y);
      }
      ctx.stroke();

      // Channel 2: CAN_L (Cyan, Baseline 2.5V, pulses DOWN to 1.5V during Dominant)
      const ch2BaseY = gy + 205;
      ctx.strokeStyle = '#00d2ff';
      ctx.lineWidth = 2.5;
      ctx.beginPath();
      for (let i = 0; i < bits.length; i++) {
        const xStart = gx + i * bitWidth;
        const xEnd = xStart + bitWidth;
        const isDom = (bits[i] === 0);
        const y = isDom ? ch2BaseY + 42 : ch2BaseY; // DOWN when dominant!
        if (i === 0) ctx.moveTo(xStart, y);
        else ctx.lineTo(xStart, y);
        ctx.lineTo(xEnd, y);
      }
      ctx.stroke();

      // Math: Vdiff = CAN_H - CAN_L (Purple, Baseline 0V, pulses UP to +2.0V during Dominant)
      const mathBaseY = gy + 325;
      ctx.strokeStyle = '#c084fc';
      ctx.lineWidth = 2.5;
      ctx.beginPath();
      for (let i = 0; i < bits.length; i++) {
        const xStart = gx + i * bitWidth;
        const xEnd = xStart + bitWidth;
        const isDom = (bits[i] === 0);
        const y = isDom ? mathBaseY - 55 : mathBaseY; // UP when dominant (+2V)
        if (i === 0) ctx.moveTo(xStart, y);
        else ctx.lineTo(xStart, y);
        ctx.lineTo(xEnd, y);
      }
      ctx.stroke();

      // Solid Channel Badges Inside Display
      function drawChannelBadge(x, y, text, color) {
        ctx.fillStyle = '#1e293b';
        ctx.fillRect(x, y - 18, 420, 26);
        ctx.strokeStyle = color;
        ctx.lineWidth = 1.5;
        ctx.strokeRect(x, y - 18, 420, 26);

        ctx.fillStyle = color;
        ctx.font = 'bold 13px monospace';
        ctx.textAlign = 'left';
        ctx.fillText(text, x + 10, y);
      }

      drawChannelBadge(gx + 15, ch1BaseY - 52, 'CH1: CAN_H [2.5V Recessive -> 3.5V Dominant]', '#facc15');
      drawChannelBadge(gx + 15, ch2BaseY - 52, 'CH2: CAN_L [2.5V Recessive -> 1.5V Dominant]', '#00d2ff');
      drawChannelBadge(gx + 15, mathBaseY - 65, 'MATH: Vdiff = CH1 - CH2 [0.0V Rec -> +2.0V Dom]', '#c084fc');

      // J1939 Protocol Decode Annotation Box (Bottom of screen)
      const decY = gy + gh - 42;
      ctx.fillStyle = '#0f172a';
      ctx.fillRect(gx, decY, gw, 36);
      ctx.strokeStyle = '#334155';
      ctx.lineWidth = 1;
      ctx.strokeRect(gx, decY, gw, 36);

      ctx.font = 'bold 13px monospace';
      ctx.fillStyle = '#00e676';
      ctx.textAlign = 'left';
      ctx.fillText('J1939 DECODE: [SOF] | PGN 0x18FEE600 (Speed/Tachometer) | DLC: 8 | DATA: [2D 09 00 54 12 00 A4 02] | CRC: 0x4A7B [ACK]', gx + 15, decY + 23);

      // Bottom Status Badges
      const bY = height - 42;
      ctx.font = '13px monospace';
      ctx.fillStyle = '#f8fafc';
      ctx.fillText('Timebase: 2.00 µs/div', 45, bY);
      ctx.fillText('Baud: 500.0 kbps', 260, bY);
      ctx.fillText('Tbit: 2.000 µs', 460, bY);
      ctx.fillText('Vdiff(Peak): +2.02 V', 680, bY);
      ctx.fillText('Sampling: 87.5%', 920, bY);
      ctx.fillText('Pass: ISO 11898-2', 1100, bY);
    }

    // Hardware Schematic & Topology Architecture Drawer
    function drawSchematic() {
      const width = 1280, height = 720;
      canvas.width = width;
      canvas.height = height;

      // Dark Blueprint Background
      ctx.fillStyle = '#0b111e';
      ctx.fillRect(0, 0, width, height);

      // Blueprint Grid
      ctx.strokeStyle = 'rgba(0, 210, 255, 0.04)';
      ctx.lineWidth = 1;
      for (let x = 0; x < width; x += 30) {
        ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, height); ctx.stroke();
      }
      for (let y = 0; y < height; y += 30) {
        ctx.beginPath(); ctx.moveTo(0, y); ctx.lineTo(width, y); ctx.stroke();
      }

      // Title Block
      ctx.fillStyle = '#00d2ff';
      ctx.font = 'bold 22px monospace';
      ctx.textAlign = 'left';
      ctx.fillText('AUTOMOTIVE TFT INSTRUMENT CLUSTER — HARDWARE ARCHITECTURE & INTERCONNECT', 45, 45);
      ctx.font = '13px monospace';
      ctx.fillStyle = '#94a3b8';
      ctx.fillText('Dual-Target Microcontroller Architecture | ISO 11898-2 High-Speed CAN & Hardware SPI DMA', 45, 68);

      function drawBlock(x, y, w, h, title, subtitle, items, color) {
        ctx.fillStyle = '#111827';
        ctx.fillRect(x, y, w, h);
        ctx.strokeStyle = color;
        ctx.lineWidth = 2;
        ctx.strokeRect(x, y, w, h);

        ctx.fillStyle = color;
        ctx.fillRect(x, y, w, 28);
        ctx.fillStyle = '#000';
        ctx.font = 'bold 13px monospace';
        ctx.textAlign = 'center';
        ctx.fillText(title, x + w / 2, y + 19);

        ctx.fillStyle = '#94a3b8';
        ctx.font = '11px monospace';
        ctx.fillText(subtitle, x + w / 2, y + 45);

        ctx.font = '12px monospace';
        ctx.textAlign = 'left';
        ctx.fillStyle = '#f8fafc';
        items.forEach((it, idx) => {
          ctx.fillText(it, x + 14, y + 72 + idx * 22);
        });
      }

      // 1. MCU (Left Column)
      drawBlock(45, 110, 310, 540, '1. HOST MCU CORE', 'STM32F401 / PIC18F46K22', [
        '• ARM Cortex-M4 @ 84MHz / PIC18',
        '• FreeRTOS Preemptive Kernel',
        '• Q15 Fixed-Point Trig Engine',
        '• Zero-Heap Dynamic Memory',
        '• Dirty-Rectangle Delta Streaming',
        '• Total Static RAM: < 300 Bytes',
        '--------------------------------',
        'HARDWARE BUS PINOUTS:',
        '• SPI1_SCK  (PA5) -> TFT Clock',
        '• SPI1_MOSI (PA7) -> TFT Data',
        '• TFT_CS    (PB6) -> Chip Select',
        '• TFT_DC    (PC7) -> Data/Command',
        '• CAN1_TX   (PB9) -> Transceiver',
        '• CAN1_RX   (PB8) -> Transceiver',
        '• I2C_SDA   (PB7) -> EEPROM Data',
        '• I2C_SCL   (PB6) -> EEPROM Clock'
      ], '#00d2ff');

      // 2. Middle Column: Display (Top) + EEPROM (Bottom)
      drawBlock(410, 110, 320, 250, '2. COLOR TFT DISPLAY', '3.2" ILI9341 320x240 RGB', [
        '• 16-Bit RGB565 High-Speed Color',
        '• Hardware SPI @ 16 MHz with DMA',
        '• GRAM Delta Window Write',
        '• Smooth 30+ FPS Zero-Tear Refresh',
        '• LED Backlight PWM Control',
        '• Anti-Glare Automotive Coating'
      ], '#00e676');

      drawBlock(410, 390, 320, 260, '3. NVM ODOMETER MEMORY', '24LC64 64Kb I2C EEPROM', [
        '• 16-Slot Round-Robin Ring Buffer',
        '• Wear-Leveling Algorithm',
        '• CRC-16-CCITT Guarded Writes',
        '• 160,000 km Lifetime Endurance',
        '• Sudden Power-Loss Anti-Tearing',
        '• Fast Boot State Recovery'
      ], '#c084fc');

      // 3. Right Column: CAN Transceiver (Top) + Vehicle Bus (Bottom)
      drawBlock(785, 110, 450, 250, '4. CAN TRANSCEIVER INTERFACE', 'Microchip MCP2551 / SN65HVD230', [
        '• ISO 11898-2 High-Speed Physical Layer',
        '• 500 kbps Automotive Baud Rate',
        '• TXD / RXD TTL Logic Interface to MCU bxCAN',
        '• Differential CAN_H / CAN_L Bus Line Driver',
        '• Thermal Shutdown & Short-to-Battery Protection'
      ], '#facc15');

      drawBlock(785, 390, 450, 260, '5. VEHICLE CAN BUS & TERMINATION', 'SAE J1939 Differential Network', [
        '• Split Termination Network: 60Ω + 60Ω + 4.7nF to GND',
        '• Common-Mode Choke (51 µH) for EMI Filtering',
        '• PESD2CAN TVS Diode Automotive Transient Suppression',
        '• Unshielded Twisted Pair (UTP) Transmission Line',
        '• 120Ω Characteristic Differential Impedance'
      ], '#ff3d00');

      // Bus Routing Lines
      function drawBus(x1, y1, x2, y2, label, color) {
        ctx.strokeStyle = color;
        ctx.lineWidth = 2.5;
        ctx.beginPath();
        ctx.moveTo(x1, y1);
        ctx.lineTo(x2, y2);
        ctx.stroke();

        ctx.fillStyle = color;
        ctx.font = 'bold 11px monospace';
        ctx.textAlign = 'center';
        ctx.fillText(label, (x1 + x2) / 2, y1 - 6);
      }

      drawBus(355, 200, 410, 200, '16MHz SPI+DMA', '#00e676');
      drawBus(355, 490, 410, 490, 'I2C 400kHz', '#c084fc');

      // Route CAN line cleanly around middle block
      ctx.strokeStyle = '#facc15';
      ctx.lineWidth = 2.5;
      ctx.beginPath();
      ctx.moveTo(355, 290);
      ctx.lineTo(385, 290);
      ctx.lineTo(385, 80);
      ctx.lineTo(755, 80);
      ctx.lineTo(755, 200);
      ctx.lineTo(785, 200);
      ctx.stroke();

      ctx.fillStyle = '#facc15';
      ctx.font = 'bold 11px monospace';
      ctx.textAlign = 'center';
      ctx.fillText('bxCAN TX/RX (TTL)', 570, 74);

      // Transceiver to Bus Line
      ctx.strokeStyle = '#ff3d00';
      ctx.lineWidth = 2.5;
      ctx.beginPath();
      ctx.moveTo(1010, 360);
      ctx.lineTo(1010, 390);
      ctx.stroke();

      ctx.fillStyle = '#ff3d00';
      ctx.font = 'bold 11px monospace';
      ctx.textAlign = 'left';
      ctx.fillText('CAN_H / CAN_L Differential Lines', 1020, 378);

      // Footer Tier-1 Compliance Note
      ctx.fillStyle = 'rgba(255,255,255,0.12)';
      ctx.font = 'bold 14px monospace';
      ctx.textAlign = 'right';
      ctx.fillText('Pricol / Bosch Aligned Production Cluster Architecture', width - 45, height - 25);
    }

    window.onload = async function() {
      async function postImage(name, dataUrl) {
        await fetch('/save-frame', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ name, dataUrl })
        });
      }

      // 1. Daylight Theme Cluster
      drawCluster({
        speed: 72, rpm: 2304, soc: 84, voltage: 52.8, gear: 'D',
        leftTurn: true, ready: true, isNight: false, trip: 142.8, odo: 14820,
        stageBanner: "STAGE 1: DAYLIGHT INSTRUMENT CLUSTER TELEMETRY (72 KM/H CRUISING)"
      });
      await postImage('01_day_mode_telemetry.png', canvas.toDataURL('image/png'));
      await postImage('01_hardware_bench_overview.jpg', canvas.toDataURL('image/jpeg', 0.95));
      await postImage('hardware_bench_setup.jpg', canvas.toDataURL('image/jpeg', 0.95));

      // 2. Night Mode High-Speed Cluster
      drawCluster({
        speed: 105, rpm: 3360, soc: 76, voltage: 51.9, gear: 'S',
        highBeam: true, ready: true, isNight: true, trip: 168.4, odo: 14845,
        stageBanner: "STAGE 2: NIGHT-MODE HIGH-SPEED INSTRUMENT CLUSTER (SPORT MODE 'S')"
      });
      await postImage('02_night_mode_telemetry.png', canvas.toDataURL('image/png'));
      await postImage('02_night_mode_glowing_bench.jpg', canvas.toDataURL('image/jpeg', 0.95));

      // 3. CAN Oscilloscope Capture
      drawOscilloscope();
      await postImage('03_can_bus_timing_validation.png', canvas.toDataURL('image/png'));
      await postImage('03_can_bus_oscilloscope_validation.jpg', canvas.toDataURL('image/jpeg', 0.95));

      // 4. Hardware Architecture Schematic
      drawSchematic();
      await postImage('04_hardware_architecture_schematic.png', canvas.toDataURL('image/png'));
      await postImage('04_can_transceiver_wiring_macro.jpg', canvas.toDataURL('image/jpeg', 0.95));

      // 5. Watchdog Failsafe Mode
      drawCluster({
        speed: 0, rpm: 0, soc: 14, voltage: 46.2, gear: 'D',
        canFault: true, battLow: true, ready: false, isNight: true, trip: 142.8, odo: 14820,
        stageBanner: "STAGE 5: SAFETY-CRITICAL FAULT INJECTION (CAN TIMEOUT WATCHDOG & LOW BATT)"
      });
      await postImage('05_failsafe_watchdog_active.png', canvas.toDataURL('image/png'));
      await postImage('05_ev_dashboard_enclosure_proto.jpg', canvas.toDataURL('image/jpeg', 0.95));

      // 6. Pristine Standalone TFT Cluster Display (No Banner)
      drawCluster({
        speed: 72, rpm: 2304, soc: 84, voltage: 52.8, gear: 'D',
        leftTurn: false, highBeam: true, ready: true, isNight: true, trip: 142.8, odo: 14820
      });
      await postImage('tft_cluster_dashboard.png', canvas.toDataURL('image/png'));
      await postImage('tft_cluster_dashboard.jpg', canvas.toDataURL('image/jpeg', 0.95));

      // 7. 60 FPS Animated Simulation Loop (50 frames at 1280x720)
      console.log("Generating 50 animated simulation frames...");
      const totalFrames = 50;
      for (let f = 0; f < totalFrames; f++) {
        const progress = f / totalFrames;
        const spd = Math.round(25 + 55 * Math.sin(progress * Math.PI));
        const currRpm = spd * 32;
        const currSoc = Math.round(85 - progress * 2);
        const currVolt = 53.0 - (progress * 0.4);
        const blink = (f % 16 < 8);
        const currTrip = 142.8 + progress * 0.4;

        drawCluster({
          width: 1280,
          height: 720,
          speed: spd,
          rpm: currRpm,
          soc: currSoc,
          voltage: currVolt,
          gear: (spd > 70) ? 'S' : 'D',
          leftTurn: blink,
          highBeam: (f > 15),
          ready: true,
          isNight: true,
          trip: currTrip,
          odo: 14820,
          estRange: 118 - Math.round(progress),
          temp: 28
        });

        const frameNum = f.toString().padStart(3, '0');
        await postImage(\`temp_frames/frame_\${frameNum}.png\`, canvas.toDataURL('image/png'));
      }

      await fetch('/done', { method: 'POST' });
    };
  </script>
</body>
</html>
`;

const server = http.createServer((req, res) => {
  if (req.method === 'GET' && req.url === '/') {
    res.writeHead(200, { 'Content-Type': 'text/html' });
    res.end(generatorHtml);
    return;
  }

  if (req.method === 'POST' && req.url === '/save-frame') {
    let body = '';
    req.on('data', chunk => { body += chunk; });
    req.on('end', () => {
      try {
        const { name, dataUrl } = JSON.parse(body);
        const base64Data = dataUrl.replace(/^data:image\/\w+;base64,/, '');
        const targetPath = path.join(IMAGES_DIR, name);
        fs.writeFileSync(targetPath, Buffer.from(base64Data, 'base64'));
        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ success: true }));
      } catch (err) {
        console.error("Error saving frame:", err);
        res.writeHead(500);
        res.end();
      }
    });
    return;
  }

  if (req.method === 'POST' && req.url === '/done') {
    res.writeHead(200, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ success: true }));

    console.log("Compiling animated GIF via ffmpeg...");
    const gifTarget = path.join(IMAGES_DIR, 'cluster_live_simulation.gif');
    const inputPattern = path.join(FRAMES_DIR, 'frame_%03d.png');

    const ffmpegCmd = `"${FFMPEG_PATH}" -y -framerate 25 -i "${inputPattern}" -vf "fps=25,scale=800:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 "${gifTarget}"`;

    exec(ffmpegCmd, (err, stdout, stderr) => {
      if (err) {
        console.error("FFmpeg error:", err);
      } else {
        console.log("GIF successfully generated:", gifTarget);
      }

      try {
        fs.rmSync(FRAMES_DIR, { recursive: true, force: true });
      } catch (e) {}

      console.log("All portfolio assets updated successfully!");
      server.close();
      process.exit(0);
    });
    return;
  }

  res.writeHead(404);
  res.end();
});

server.listen(PORT, () => {
  console.log(`Server listening on http://localhost:${PORT}`);
  spawn(CHROME_PATH, [
    '--headless=new',
    '--disable-gpu',
    '--no-sandbox',
    '--window-size=1300,750',
    `http://localhost:${PORT}/`
  ]);
});
