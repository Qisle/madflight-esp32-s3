#pragma once

#include <Arduino.h>

static const char USR_MOBILE_UI_PAGE[] PROGMEM = R"USRUI(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
  <title>Madflight Cockpit</title>
  <style>
    :root {
      --bg: #041016;
      --panel: rgba(7, 18, 24, 0.76);
      --panel-strong: rgba(8, 14, 18, 0.94);
      --panel-soft: rgba(9, 17, 22, 0.58);
      --edge: rgba(214, 236, 245, 0.14);
      --edge-strong: rgba(255, 211, 87, 0.34);
      --text: #f4f8fb;
      --muted: #8fa6b0;
      --accent: #f3c237;
      --accent-soft: rgba(243, 194, 55, 0.18);
      --teal: #30e4c5;
      --danger: #ff4d4d;
      --good: #6df9a0;
      --shadow: 0 26px 60px rgba(0, 0, 0, 0.38);
      --radius: 18px;
      --radius-lg: 24px;
      --radius-sm: 12px;
      --font-ui: "Segoe UI", sans-serif;
      --font-display: "Bahnschrift", "Trebuchet MS", sans-serif;
    }

    * { box-sizing: border-box; }
    html, body {
      margin: 0;
      width: 100%;
      height: 100%;
      overflow: hidden;
      background:
        radial-gradient(circle at top left, rgba(48,228,197,0.12), transparent 28%),
        radial-gradient(circle at top right, rgba(243,194,55,0.16), transparent 26%),
        linear-gradient(180deg, #10242c 0%, #071219 48%, #03080c 100%);
      color: var(--text);
      font-family: var(--font-ui);
      overscroll-behavior: none;
      touch-action: manipulation;
    }

    body.menu-open {
      overflow: hidden;
    }

    button, input {
      font: inherit;
    }

    .viewport {
      position: fixed;
      inset: 0;
      overflow: hidden;
      background:
        radial-gradient(circle at 50% 50%, rgba(255,255,255,0.04), transparent 42%),
        linear-gradient(180deg, rgba(255,255,255,0.02), rgba(0,0,0,0.16));
    }

    .video-stage {
      position: absolute;
      inset: 0;
      overflow: hidden;
      background:
        radial-gradient(circle at center, rgba(40, 154, 170, 0.34), transparent 46%),
        linear-gradient(180deg, rgba(14, 31, 38, 0.92), rgba(6, 14, 18, 0.96));
    }

    .video-feed,
    .video-vignette,
    .video-grid {
      position: absolute;
      inset: 0;
    }

    .video-feed {
      display: grid;
      place-items: center;
      overflow: hidden;
      background:
        radial-gradient(circle at center, rgba(26, 170, 186, 0.36), transparent 42%),
        linear-gradient(180deg, rgba(18, 70, 84, 0.86), rgba(10, 42, 54, 0.9));
    }

    .video-feed img,
    .video-feed video,
    .video-feed iframe {
      width: 100%;
      height: 100%;
      object-fit: cover;
      border: 0;
    }

    .video-placeholder {
      width: min(72vw, 760px);
      aspect-ratio: 16 / 9;
      border-radius: 28px;
      border: 1px dashed rgba(255,255,255,0.18);
      background:
        linear-gradient(180deg, rgba(255,255,255,0.05), rgba(255,255,255,0.015)),
        radial-gradient(circle at center, rgba(48,228,197,0.12), transparent 46%);
      display: grid;
      place-items: center;
      padding: 28px;
      text-align: center;
      box-shadow: inset 0 0 60px rgba(0,0,0,0.16);
    }

    .video-placeholder strong {
      display: block;
      font-family: var(--font-display);
      font-size: clamp(1.2rem, 3vw, 2rem);
      letter-spacing: 0.06em;
      margin-bottom: 10px;
    }

    .video-note {
      max-width: 44ch;
      color: rgba(244, 248, 251, 0.78);
      line-height: 1.55;
      font-size: 0.98rem;
    }

    .video-vignette {
      background:
        linear-gradient(180deg, rgba(0,0,0,0.28), transparent 24%, transparent 76%, rgba(0,0,0,0.34)),
        radial-gradient(circle at center, transparent 44%, rgba(0,0,0,0.26) 100%);
      pointer-events: none;
    }

    .video-grid {
      inset: 84px 22px 118px 22px;
      border: 1px solid rgba(255,255,255,0.12);
      background:
        linear-gradient(rgba(255,255,255,0.24), rgba(255,255,255,0.24)) 0 33.333% / 100% 1px no-repeat,
        linear-gradient(rgba(255,255,255,0.24), rgba(255,255,255,0.24)) 0 66.666% / 100% 1px no-repeat,
        linear-gradient(90deg, rgba(255,255,255,0.24), rgba(255,255,255,0.24)) 33.333% 0 / 1px 100% no-repeat,
        linear-gradient(90deg, rgba(255,255,255,0.24), rgba(255,255,255,0.24)) 66.666% 0 / 1px 100% no-repeat;
      pointer-events: none;
      opacity: 0.8;
    }

    .hud-layer {
      position: absolute;
      inset: 0;
      pointer-events: none;
    }

    .glass,
    .top-chip,
    .menu-button,
    .mini-card,
    .alt-scale,
    .action-orb,
    .right-meta,
    .signal-card,
    .compass-card,
    .bottom-panel,
    .menu-drawer,
    .menu-tab,
    .menu-card,
    .param-row,
    .wave-wrap,
    .toast {
      backdrop-filter: blur(18px);
      -webkit-backdrop-filter: blur(18px);
      box-shadow: var(--shadow);
      border: 1px solid var(--edge);
    }

    .hud-top {
      position: absolute;
      top: 14px;
      left: 14px;
      right: 14px;
      display: grid;
      grid-template-columns: minmax(220px, 26vw) 1fr auto;
      align-items: start;
      gap: 14px;
      pointer-events: none;
    }

    .brand-ribbon {
      min-height: 68px;
      display: grid;
      grid-template-columns: auto 1fr;
      align-items: stretch;
      background: linear-gradient(90deg, rgba(7, 17, 24, 0.92), rgba(57, 48, 8, 0.78));
      border-radius: var(--radius-lg);
      overflow: hidden;
      border: 1px solid rgba(255,255,255,0.08);
      box-shadow: var(--shadow);
    }

    .brand-mark {
      min-width: 82px;
      display: grid;
      place-items: center;
      font-family: var(--font-display);
      font-size: 1.55rem;
      font-weight: 800;
      letter-spacing: 0.08em;
      color: #ffffff;
      background:
        linear-gradient(135deg, rgba(255,255,255,0.12), rgba(255,255,255,0.04));
    }

    .mission-block {
      position: relative;
      display: grid;
      align-content: center;
      gap: 4px;
      padding: 12px 18px 12px 22px;
      background:
        linear-gradient(100deg, rgba(243,194,55,0.9) 0 18px, rgba(86,74,18,0.76) 18px);
      clip-path: polygon(0 0, 100% 0, 100% 100%, 18px 100%, 0 50%);
    }

    .mission-title {
      font-family: var(--font-display);
      font-size: 0.72rem;
      font-weight: 800;
      letter-spacing: 0.22em;
      color: rgba(10, 16, 22, 0.75);
    }

    .mission-text {
      font-family: var(--font-display);
      font-size: clamp(0.95rem, 2vw, 1.42rem);
      font-weight: 800;
      color: #ffffff;
      letter-spacing: 0.03em;
      text-shadow: 0 2px 12px rgba(0,0,0,0.18);
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
    }

    .camera-strip {
      min-height: 68px;
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 10px;
      align-items: stretch;
    }

    .cam-stat {
      background: linear-gradient(180deg, rgba(6, 14, 18, 0.9), rgba(6, 11, 16, 0.72));
      border-radius: var(--radius);
      padding: 12px 14px;
      border: 1px solid rgba(255,255,255,0.08);
      display: grid;
      align-content: center;
      gap: 5px;
      min-width: 0;
      box-shadow: var(--shadow);
    }

    .cam-stat span {
      font-size: 0.72rem;
      text-transform: uppercase;
      letter-spacing: 0.16em;
      color: var(--muted);
    }

    .cam-stat strong {
      font-family: var(--font-display);
      font-size: clamp(1rem, 2vw, 1.48rem);
      font-weight: 800;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
    }

    .top-right-cluster {
      display: flex;
      align-items: center;
      gap: 10px;
      justify-self: end;
      pointer-events: auto;
    }

    .top-chip {
      min-height: 54px;
      padding: 10px 14px;
      border-radius: 999px;
      background: rgba(7, 18, 24, 0.68);
      display: inline-flex;
      align-items: center;
      gap: 10px;
      color: var(--muted);
    }

    .top-chip span,
    .top-chip strong {
      white-space: nowrap;
    }

    .top-chip strong {
      color: var(--text);
      font-family: var(--font-display);
      font-size: 1rem;
      letter-spacing: 0.04em;
    }

    .chip-dot {
      width: 10px;
      height: 10px;
      border-radius: 999px;
      background: var(--danger);
      box-shadow: 0 0 16px rgba(255,77,77,0.65);
      flex: 0 0 auto;
    }

    .chip-dot.ok {
      background: var(--good);
      box-shadow: 0 0 16px rgba(109,249,160,0.65);
    }

    .menu-button {
      width: 56px;
      height: 56px;
      border-radius: 999px;
      background: rgba(7, 18, 24, 0.82);
      display: inline-grid;
      place-items: center;
      cursor: pointer;
      pointer-events: auto;
    }

    .menu-button span {
      width: 18px;
      height: 2px;
      border-radius: 999px;
      background: var(--text);
      display: block;
      box-shadow: 0 6px 0 var(--text), 0 -6px 0 var(--text);
    }

    .hud-left {
      position: absolute;
      top: 104px;
      left: 18px;
      width: min(220px, 26vw);
      display: grid;
      gap: 12px;
      align-content: start;
      pointer-events: none;
    }

    .mini-card,
    .signal-card,
    .compass-card,
    .alt-scale,
    .right-meta,
    .bottom-panel {
      background: linear-gradient(180deg, rgba(6, 14, 18, 0.82), rgba(5, 10, 14, 0.62));
      border-radius: var(--radius);
      overflow: hidden;
    }

    .mini-card {
      padding: 14px 15px;
      display: grid;
      gap: 6px;
    }

    .mini-label {
      font-size: 0.7rem;
      text-transform: uppercase;
      letter-spacing: 0.18em;
      color: var(--muted);
    }

    .mini-card strong {
      font-family: var(--font-display);
      font-size: 1rem;
      font-weight: 800;
      letter-spacing: 0.04em;
    }

    .mini-card small {
      color: rgba(244, 248, 251, 0.72);
      font-size: 0.85rem;
      line-height: 1.4;
      word-break: break-all;
    }

    .signal-card {
      position: absolute;
      top: 184px;
      left: calc(min(220px, 26vw) + 42px);
      width: min(240px, 22vw);
      padding: 14px;
      display: grid;
      gap: 10px;
      pointer-events: none;
    }

    .signal-head {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 10px;
    }

    .signal-head span {
      font-size: 0.72rem;
      letter-spacing: 0.16em;
      text-transform: uppercase;
      color: var(--muted);
    }

    .signal-bars {
      height: 96px;
      border-radius: var(--radius-sm);
      border: 1px solid rgba(255,255,255,0.08);
      background:
        linear-gradient(rgba(255,255,255,0.08), rgba(255,255,255,0.08)) 0 25% / 100% 1px no-repeat,
        linear-gradient(rgba(255,255,255,0.08), rgba(255,255,255,0.08)) 0 50% / 100% 1px no-repeat,
        linear-gradient(rgba(255,255,255,0.08), rgba(255,255,255,0.08)) 0 75% / 100% 1px no-repeat,
        linear-gradient(90deg, rgba(255,255,255,0.06), rgba(255,255,255,0.06)) 20% 0 / 1px 100% no-repeat,
        linear-gradient(90deg, rgba(255,255,255,0.06), rgba(255,255,255,0.06)) 40% 0 / 1px 100% no-repeat,
        linear-gradient(90deg, rgba(255,255,255,0.06), rgba(255,255,255,0.06)) 60% 0 / 1px 100% no-repeat,
        linear-gradient(90deg, rgba(255,255,255,0.06), rgba(255,255,255,0.06)) 80% 0 / 1px 100% no-repeat,
        linear-gradient(180deg, rgba(255,255,255,0.03), rgba(255,255,255,0.01));
      position: relative;
      overflow: hidden;
    }

    .signal-bars::after {
      content: "";
      position: absolute;
      inset: 10px 24px 12px 28px;
      background:
        radial-gradient(circle at 28% 82%, rgba(255,255,255,0.92), transparent 16%),
        radial-gradient(circle at 40% 54%, rgba(255,255,255,0.92), transparent 22%),
        radial-gradient(circle at 46% 36%, rgba(255,255,255,0.92), transparent 14%),
        linear-gradient(180deg, rgba(255,255,255,0.86), rgba(255,255,255,0.18));
      clip-path: polygon(0 100%, 16% 94%, 24% 80%, 36% 36%, 52% 12%, 60% 0, 70% 100%);
      opacity: 0.9;
    }

    .signal-values {
      display: grid;
      gap: 6px;
      color: rgba(244,248,251,0.78);
      font-size: 0.85rem;
    }

    .hud-right {
      position: absolute;
      top: 128px;
      right: 18px;
      width: min(156px, 18vw);
      display: grid;
      gap: 14px;
      align-content: start;
      pointer-events: none;
    }

    .alt-scale {
      padding: 14px 16px 16px;
      display: grid;
      gap: 12px;
    }

    .alt-scale span {
      font-size: 0.72rem;
      text-transform: uppercase;
      letter-spacing: 0.16em;
      color: var(--muted);
    }

    .scale-core {
      min-height: 250px;
      display: grid;
      justify-items: center;
      gap: 14px;
    }

    .scale-track {
      position: relative;
      width: 48px;
      height: 190px;
      border-radius: 999px;
      background:
        repeating-linear-gradient(180deg,
          rgba(255,255,255,0.14) 0 4px,
          transparent 4px 18px);
      border: 1px solid rgba(255,255,255,0.08);
    }

    .scale-track::before,
    .scale-track::after {
      content: "";
      position: absolute;
      left: 50%;
      transform: translateX(-50%);
      width: 14px;
      height: 14px;
      border-radius: 999px;
      border: 2px solid rgba(255,255,255,0.8);
      background: rgba(255,255,255,0.06);
    }

    .scale-track::before { top: 10px; }
    .scale-track::after { bottom: 10px; }

    .scale-readout {
      font-family: var(--font-display);
      font-size: 1.5rem;
      font-weight: 800;
      padding: 7px 10px;
      border-radius: 14px;
      background: rgba(255,255,255,0.08);
      color: #ffffff;
      min-width: 94px;
      text-align: center;
    }

    .scale-note {
      font-size: 0.9rem;
      color: rgba(244,248,251,0.72);
      text-align: center;
    }

    .action-orb {
      height: 112px;
      border-radius: 999px;
      display: grid;
      place-items: center;
      background:
        radial-gradient(circle at center, rgba(255,77,77,0.96) 0 38%, rgba(116,0,0,0.88) 39% 40%, transparent 41%),
        radial-gradient(circle at center, rgba(255,255,255,0.26) 0 47%, transparent 48%),
        rgba(7,18,24,0.76);
      font-family: var(--font-display);
      font-size: 1.25rem;
      font-weight: 800;
      letter-spacing: 0.06em;
      text-align: center;
      padding: 12px;
    }

    .right-meta {
      padding: 12px 14px;
      display: grid;
      gap: 10px;
    }

    .right-row {
      display: flex;
      justify-content: space-between;
      gap: 8px;
      color: var(--muted);
      font-size: 0.86rem;
    }

    .right-row strong {
      color: var(--text);
      font-family: var(--font-display);
      font-size: 0.96rem;
      letter-spacing: 0.04em;
    }

    .center-reticle {
      position: absolute;
      inset: 0;
      display: grid;
      place-items: center;
      pointer-events: none;
    }

    .crosshair {
      position: relative;
      width: 184px;
      height: 184px;
    }

    .crosshair::before,
    .crosshair::after {
      content: "";
      position: absolute;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      background: rgba(255,255,255,0.22);
    }

    .crosshair::before { width: 184px; height: 1px; }
    .crosshair::after { width: 1px; height: 184px; }

    .crosshair-ring {
      position: absolute;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      width: 46px;
      height: 46px;
      border-radius: 999px;
      border: 2px solid rgba(255,77,77,0.88);
      box-shadow: 0 0 0 10px rgba(255,77,77,0.05);
    }

    .crosshair-ring::before,
    .crosshair-ring::after {
      content: "";
      position: absolute;
      left: 50%;
      top: 50%;
      background: rgba(255,77,77,0.88);
      transform: translate(-50%, -50%);
    }

    .crosshair-ring::before { width: 14px; height: 2px; }
    .crosshair-ring::after { width: 2px; height: 14px; }

    .focus-dot {
      position: absolute;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      width: 7px;
      height: 7px;
      border-radius: 999px;
      background: rgba(255,77,77,0.96);
      box-shadow: 0 0 16px rgba(255,77,77,0.66);
    }

    .center-tag {
      position: absolute;
      left: 50%;
      bottom: 24%;
      transform: translateX(-50%);
      min-width: 170px;
      padding: 10px 16px;
      border-radius: 18px;
      background: linear-gradient(180deg, rgba(133,20,20,0.72), rgba(40,5,5,0.52));
      border: 1px solid rgba(255,255,255,0.1);
      display: grid;
      justify-items: center;
      gap: 4px;
    }

    .center-tag strong {
      font-family: var(--font-display);
      font-size: 1.34rem;
      font-weight: 800;
      letter-spacing: 0.08em;
    }

    .center-tag span {
      color: rgba(244,248,251,0.74);
      font-size: 0.82rem;
      letter-spacing: 0.12em;
      text-transform: uppercase;
    }

    .compass-card {
      position: absolute;
      left: 18px;
      bottom: 22px;
      width: min(226px, 24vw);
      min-height: 192px;
      padding: 16px;
      display: grid;
      gap: 14px;
      pointer-events: none;
    }

    .compass-visual {
      position: relative;
      width: 154px;
      height: 154px;
      border-radius: 999px;
      margin: 0 auto;
      border: 1px solid rgba(255,255,255,0.14);
      background:
        radial-gradient(circle at center, rgba(255,255,255,0.06) 0 18%, transparent 19% 100%),
        radial-gradient(circle at center, transparent 0 34%, rgba(255,255,255,0.08) 34.5% 35.5%, transparent 36% 100%),
        radial-gradient(circle at center, transparent 0 52%, rgba(255,255,255,0.08) 52.5% 53.5%, transparent 54% 100%),
        linear-gradient(180deg, rgba(255,255,255,0.08), rgba(255,255,255,0.02));
      overflow: hidden;
    }

    .compass-visual::before,
    .compass-visual::after {
      content: "";
      position: absolute;
      left: 50%;
      top: 50%;
      transform: translate(-50%, -50%);
      background: rgba(255,255,255,0.14);
    }

    .compass-visual::before { width: 126px; height: 1px; }
    .compass-visual::after { width: 1px; height: 126px; }

    .compass-needle {
      position: absolute;
      left: 50%;
      top: 14px;
      width: 0;
      height: 62px;
      transform-origin: 50% 63px;
      transform: translateX(-50%) rotate(0deg);
    }

    .compass-needle::before {
      content: "";
      position: absolute;
      left: 50%;
      top: 0;
      transform: translateX(-50%);
      width: 18px;
      height: 72px;
      clip-path: polygon(50% 0, 100% 100%, 50% 78%, 0 100%);
      background: linear-gradient(180deg, rgba(255,0,0,0.96), rgba(255,92,92,0.72));
      box-shadow: 0 0 24px rgba(255,0,0,0.24);
    }

    .compass-visual .north-tag {
      position: absolute;
      top: 8px;
      left: 50%;
      transform: translateX(-50%);
      width: 28px;
      height: 28px;
      border-radius: 999px;
      background: rgba(0,0,0,0.48);
      display: grid;
      place-items: center;
      font-family: var(--font-display);
      font-size: 0.86rem;
      font-weight: 800;
    }

    .compass-meta {
      display: grid;
      gap: 6px;
    }

    .compass-meta span {
      font-size: 0.7rem;
      letter-spacing: 0.16em;
      text-transform: uppercase;
      color: var(--muted);
    }

    .compass-meta strong {
      font-family: var(--font-display);
      font-size: 1rem;
      font-weight: 800;
      letter-spacing: 0.04em;
    }

    .bottom-panel {
      position: absolute;
      left: 50%;
      bottom: 22px;
      transform: translateX(-50%);
      width: min(920px, calc(100vw - 320px));
      padding: 0;
      pointer-events: none;
    }

    .summary-strip {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 1px;
      background: rgba(255,255,255,0.06);
      border-top-left-radius: var(--radius-lg);
      border-top-right-radius: var(--radius-lg);
      overflow: hidden;
    }

    .summary-cell {
      background: rgba(5,12,16,0.88);
      padding: 12px 14px;
      display: grid;
      gap: 5px;
      min-width: 0;
    }

    .summary-cell span {
      font-size: 0.72rem;
      letter-spacing: 0.16em;
      text-transform: uppercase;
      color: var(--muted);
    }

    .summary-cell strong {
      font-family: var(--font-display);
      font-size: 1rem;
      font-weight: 800;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
    }

    .motor-strip {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 10px;
      padding: 12px;
      background: rgba(3, 8, 12, 0.84);
      border-bottom-left-radius: var(--radius-lg);
      border-bottom-right-radius: var(--radius-lg);
    }

    .motor-chip {
      border-radius: 16px;
      padding: 10px 12px;
      background: rgba(255,255,255,0.04);
      border: 1px solid rgba(255,255,255,0.06);
      display: flex;
      justify-content: space-between;
      gap: 8px;
      align-items: center;
      min-width: 0;
    }

    .motor-chip span {
      color: var(--muted);
      font-size: 0.76rem;
      letter-spacing: 0.14em;
      text-transform: uppercase;
    }

    .motor-chip strong {
      font-family: var(--font-display);
      font-size: 0.98rem;
      font-weight: 800;
      letter-spacing: 0.04em;
    }

    .menu-backdrop {
      position: fixed;
      inset: 0;
      background: rgba(1, 6, 8, 0.42);
      opacity: 0;
      pointer-events: none;
      transition: opacity 180ms ease;
      z-index: 30;
    }

    .menu-backdrop.open {
      opacity: 1;
      pointer-events: auto;
    }

    .menu-drawer {
      position: fixed;
      top: 12px;
      right: 12px;
      bottom: 12px;
      width: min(460px, calc(100vw - 24px));
      background: linear-gradient(180deg, rgba(6, 13, 17, 0.98), rgba(5, 10, 14, 0.98));
      border-radius: 30px;
      transform: translateX(calc(100% + 24px));
      transition: transform 220ms ease;
      z-index: 32;
      display: grid;
      grid-template-rows: auto auto 1fr;
      overflow: hidden;
    }

    .menu-drawer.open {
      transform: translateX(0);
    }

    .menu-head {
      padding: 18px 18px 14px;
      display: flex;
      justify-content: space-between;
      gap: 12px;
      align-items: center;
      border-bottom: 1px solid rgba(255,255,255,0.07);
    }

    .menu-title-wrap {
      display: grid;
      gap: 4px;
      min-width: 0;
    }

    .menu-title {
      font-family: var(--font-display);
      font-size: 1.1rem;
      font-weight: 800;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }

    .menu-subtitle {
      color: var(--muted);
      font-size: 0.86rem;
      line-height: 1.4;
    }

    .close-button {
      min-width: 48px;
      height: 48px;
      border: 0;
      border-radius: 999px;
      background: rgba(255,255,255,0.06);
      color: var(--text);
      cursor: pointer;
      border: 1px solid rgba(255,255,255,0.08);
    }

    .menu-tabs {
      padding: 12px 18px;
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 8px;
      border-bottom: 1px solid rgba(255,255,255,0.06);
    }

    .menu-tab {
      min-height: 46px;
      border-radius: 14px;
      background: rgba(255,255,255,0.04);
      color: var(--muted);
      border: 1px solid rgba(255,255,255,0.06);
      cursor: pointer;
      font-family: var(--font-display);
      font-size: 0.84rem;
      font-weight: 700;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }

    .menu-tab.active {
      background: linear-gradient(180deg, rgba(243,194,55,0.2), rgba(243,194,55,0.08));
      color: var(--text);
      border-color: var(--edge-strong);
    }

    .menu-panels {
      position: relative;
      min-height: 0;
    }

    .menu-panel {
      position: absolute;
      inset: 0;
      padding: 16px 18px 18px;
      display: none;
      flex-direction: column;
      gap: 14px;
      min-height: 0;
    }

    .menu-panel.active {
      display: flex;
    }

    .menu-scroll {
      min-height: 0;
      overflow: auto;
      display: grid;
      gap: 14px;
      align-content: start;
      padding-right: 4px;
    }

    .menu-card {
      padding: 15px;
      border-radius: 18px;
      background: linear-gradient(180deg, rgba(10,18,24,0.84), rgba(6,12,16,0.7));
      display: grid;
      gap: 12px;
    }

    .menu-card h3 {
      margin: 0;
      font-size: 0.78rem;
      text-transform: uppercase;
      letter-spacing: 0.16em;
      color: var(--muted);
    }

    .menu-card p {
      margin: 0;
      color: rgba(244,248,251,0.76);
      line-height: 1.55;
      font-size: 0.92rem;
    }

    .info-list {
      display: grid;
      gap: 10px;
    }

    .info-row {
      display: flex;
      justify-content: space-between;
      gap: 8px;
      color: var(--muted);
      font-size: 0.9rem;
      align-items: center;
    }

    .info-row strong {
      color: var(--text);
      font-family: var(--font-display);
      font-size: 0.98rem;
      font-weight: 800;
      text-align: right;
      word-break: break-all;
    }

    .quick-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 10px;
    }

    .quick-tile {
      min-height: 88px;
      border-radius: 18px;
      border: 1px solid rgba(255,255,255,0.08);
      background: rgba(255,255,255,0.04);
      display: grid;
      align-content: center;
      gap: 6px;
      padding: 12px;
      cursor: pointer;
      color: var(--text);
      text-align: left;
    }

    .quick-tile span {
      color: var(--muted);
      font-size: 0.75rem;
      letter-spacing: 0.16em;
      text-transform: uppercase;
    }

    .quick-tile strong {
      font-family: var(--font-display);
      font-size: 1rem;
      font-weight: 800;
      letter-spacing: 0.05em;
    }

    .btn {
      min-height: 46px;
      border: 0;
      border-radius: 999px;
      padding: 0 18px;
      cursor: pointer;
      color: var(--text);
      background: linear-gradient(180deg, rgba(243,194,55,0.24), rgba(243,194,55,0.1));
      border: 1px solid rgba(243,194,55,0.32);
      font-family: var(--font-display);
      font-size: 0.84rem;
      font-weight: 800;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }

    .btn.secondary {
      background: rgba(255,255,255,0.05);
      border: 1px solid rgba(255,255,255,0.08);
    }

    .btn.warn {
      background: linear-gradient(180deg, rgba(255,77,77,0.2), rgba(255,77,77,0.08));
      border-color: rgba(255,77,77,0.28);
    }

    .searchbar {
      width: 100%;
      min-height: 46px;
      border-radius: 14px;
      border: 1px solid rgba(255,255,255,0.08);
      background: rgba(255,255,255,0.05);
      color: var(--text);
      padding: 0 14px;
      outline: none;
    }

    .param-list {
      min-height: 0;
      overflow: auto;
      display: grid;
      gap: 10px;
      padding-right: 4px;
      align-content: start;
    }

    .param-row {
      border-radius: 16px;
      background: rgba(255,255,255,0.035);
      display: grid;
      grid-template-columns: minmax(0, 1fr) 116px auto;
      gap: 10px;
      align-items: center;
      padding: 12px;
    }

    .param-name {
      display: flex;
      flex-direction: column;
      gap: 6px;
      min-width: 0;
    }

    .param-name strong {
      font-size: 0.92rem;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
    }

    .param-name span {
      color: var(--muted);
      font-size: 0.74rem;
      text-transform: uppercase;
      letter-spacing: 0.1em;
    }

    .param-row input {
      min-height: 42px;
      border-radius: 12px;
      border: 1px solid rgba(255,255,255,0.08);
      background: rgba(4,9,12,0.88);
      color: var(--text);
      padding: 0 12px;
      outline: none;
    }

    .drawer-foot {
      display: flex;
      justify-content: space-between;
      gap: 10px;
      flex-wrap: wrap;
    }

    .wave-wrap {
      min-height: 0;
      overflow: hidden;
      border-radius: 20px;
      background: linear-gradient(180deg, rgba(3,7,10,0.96), rgba(6,11,16,0.96));
      display: grid;
      grid-template-rows: minmax(260px, 1fr) auto;
    }

    #waveCanvas {
      width: 100%;
      height: 100%;
      display: block;
      min-height: 280px;
    }

    .wave-legend {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      padding: 12px;
      border-top: 1px solid rgba(255,255,255,0.06);
    }

    .wave-tag {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      padding: 7px 10px;
      border-radius: 999px;
      background: rgba(255,255,255,0.04);
      color: var(--muted);
      font-size: 0.8rem;
    }

    .wave-color {
      width: 9px;
      height: 9px;
      border-radius: 999px;
    }

    .toast {
      position: fixed;
      left: 50%;
      bottom: 18px;
      transform: translate(-50%, 18px);
      background: rgba(6, 13, 17, 0.94);
      color: var(--text);
      border-radius: 999px;
      padding: 11px 18px;
      opacity: 0;
      pointer-events: none;
      transition: opacity 160ms ease, transform 160ms ease;
      z-index: 40;
      max-width: min(92vw, 640px);
      text-align: center;
    }

    .toast.show {
      opacity: 1;
      transform: translate(-50%, 0);
    }

    @media (max-width: 1180px) {
      .hud-top {
        grid-template-columns: minmax(220px, 30vw) 1fr auto;
      }

      .signal-card {
        display: none;
      }

      .bottom-panel {
        width: min(860px, calc(100vw - 250px));
      }
    }

    @media (max-width: 920px) {
      .camera-strip {
        grid-template-columns: repeat(2, minmax(0, 1fr));
      }

      .hud-top {
        grid-template-columns: 1fr auto;
      }

      .camera-strip {
        grid-column: 1 / -1;
      }

      .hud-left {
        top: 156px;
        width: 180px;
      }

      .hud-right {
        width: 132px;
      }

      .bottom-panel {
        width: calc(100vw - 220px);
      }
    }

    @media (max-width: 720px) {
      .video-grid {
        inset: 90px 12px 138px 12px;
      }

      .hud-top {
        top: 10px;
        left: 10px;
        right: 10px;
        gap: 10px;
      }

      .brand-ribbon {
        min-height: 58px;
      }

      .brand-mark {
        min-width: 66px;
        font-size: 1.22rem;
      }

      .mission-block {
        padding: 10px 14px 10px 18px;
      }

      .mission-text {
        font-size: 0.92rem;
      }

      .camera-strip {
        gap: 8px;
      }

      .cam-stat {
        min-height: 52px;
        padding: 10px 12px;
      }

      .top-chip {
        min-height: 46px;
        padding: 8px 11px;
      }

      .top-right-cluster {
        gap: 8px;
      }

      .menu-button {
        width: 48px;
        height: 48px;
      }

      .hud-left {
        top: auto;
        left: 10px;
        bottom: 154px;
        width: 142px;
      }

      .mini-card {
        padding: 11px 12px;
      }

      .mini-card small {
        font-size: 0.75rem;
      }

      .hud-right {
        top: auto;
        right: 10px;
        bottom: 150px;
        width: 116px;
      }

      .alt-scale {
        padding: 10px 10px 12px;
      }

      .scale-core {
        min-height: 194px;
      }

      .scale-track {
        height: 144px;
      }

      .action-orb {
        height: 86px;
        font-size: 1rem;
      }

      .center-tag {
        bottom: 30%;
        min-width: 134px;
      }

      .compass-card {
        left: 10px;
        bottom: 10px;
        width: 146px;
        min-height: 148px;
        padding: 12px;
      }

      .compass-visual {
        width: 98px;
        height: 98px;
      }

      .compass-visual::before {
        width: 76px;
      }

      .compass-visual::after {
        height: 76px;
      }

      .compass-needle {
        top: 8px;
        height: 36px;
        transform-origin: 50% 40px;
      }

      .compass-needle::before {
        width: 14px;
        height: 48px;
      }

      .bottom-panel {
        left: 10px;
        right: 10px;
        bottom: 10px;
        transform: none;
        width: auto;
      }

      .summary-strip,
      .motor-strip {
        grid-template-columns: repeat(2, minmax(0, 1fr));
      }

      .menu-drawer {
        top: 8px;
        right: 8px;
        bottom: 8px;
        width: calc(100vw - 16px);
        border-radius: 24px;
      }

      .menu-tabs {
        grid-template-columns: repeat(2, minmax(0, 1fr));
      }

      .quick-grid {
        grid-template-columns: 1fr;
      }

      .param-row {
        grid-template-columns: 1fr;
      }
    }
  </style>
</head>
<body>
  <div class="viewport">
    <div class="video-stage">
      <div class="video-feed" id="videoFeed">
        <div class="video-placeholder" id="videoPlaceholder">
          <div>
            <strong>Camera Slot Reserved</strong>
            <div class="video-note" id="videoNote">
              This full-screen area is reserved for the live camera. Later you can attach MJPEG, WebRTC, video, or iframe content through window.MadflightUI.attachVideoSource(url, type).
            </div>
          </div>
        </div>
      </div>
      <div class="video-vignette"></div>
      <div class="video-grid"></div>

      <div class="hud-layer">
        <header class="hud-top">
          <div class="brand-ribbon">
            <div class="brand-mark">MF</div>
            <div class="mission-block">
              <div class="mission-title">Flight Deck</div>
              <div class="mission-text" id="footerMessage">Waiting for hotspot link</div>
            </div>
          </div>

          <div class="camera-strip">
            <div class="cam-stat"><span>Throttle</span><strong id="thrValue">0.00</strong></div>
            <div class="cam-stat"><span>Sample</span><strong id="rateValue">0 Hz</strong></div>
            <div class="cam-stat"><span>Link</span><strong id="footerLink">WAIT</strong></div>
            <div class="cam-stat"><span>Video</span><strong id="footerVideo">READY</strong></div>
          </div>

          <div class="top-right-cluster">
            <div class="top-chip"><span class="chip-dot" id="wifiDot"></span><strong id="wifiState">Booting</strong></div>
            <div class="top-chip"><span>ARM</span><strong id="armValue">DISARMED</strong></div>
            <div class="top-chip"><span>RC</span><strong id="rcValue">Offline</strong></div>
            <button class="menu-button" id="openMenuBtn" aria-label="Open menu"><span></span></button>
          </div>
        </header>

        <aside class="hud-left">
          <div class="mini-card">
            <span class="mini-label">Host</span>
            <strong id="domainValue">madflight.local</strong>
            <small id="urlValue">http://0.0.0.0/</small>
          </div>
          <div class="mini-card">
            <span class="mini-label">Attitude</span>
            <strong id="rollValue">0.0 deg</strong>
            <small id="pitchValue">0.0 deg</small>
            <small id="yawValue">0.0 deg</small>
          </div>
          <div class="mini-card">
            <span class="mini-label">Sensors</span>
            <strong id="imuName">--</strong>
            <small id="barName">--</small>
            <small id="magName">--</small>
          </div>
        </aside>

        <div class="signal-card">
          <div class="signal-head">
            <span>Motion Signal</span>
            <span>Live</span>
          </div>
          <div class="signal-bars"></div>
          <div class="signal-values">
            <div id="gyroValue">0 / 0 / 0</div>
            <div id="accelValue">0 / 0 / 0</div>
          </div>
        </div>

        <aside class="hud-right">
          <div class="alt-scale">
            <span>Altitude</span>
            <div class="scale-core">
              <div class="scale-track"></div>
              <div class="scale-readout" id="altValue">--</div>
              <div class="scale-note" id="pressValue">--</div>
            </div>
          </div>

          <div class="action-orb" id="footerArm">DISARMED</div>

          <div class="right-meta">
            <div class="right-row"><span>Camera</span><strong id="cameraState">Reserved</strong></div>
            <div class="right-row"><span>Save</span><strong id="footerSave">IDLE</strong></div>
            <div class="right-row"><span>Wave</span><strong id="waveState">Ready</strong></div>
          </div>
        </aside>

        <div class="center-reticle">
          <div class="crosshair">
            <div class="crosshair-ring"></div>
          </div>
          <div class="focus-dot"></div>
          <div class="center-tag">
            <strong id="modeValue">RATE</strong>
            <span>Live Cockpit</span>
          </div>
        </div>

        <div class="compass-card">
          <div class="compass-visual">
            <div class="north-tag">N</div>
            <div class="compass-needle" id="compassNeedle"></div>
          </div>
          <div class="compass-meta">
            <span>Heading</span>
            <strong id="yawCompassValue">0.0 deg</strong>
            <span>Mag</span>
            <strong id="magValue">0 / 0 / 0</strong>
          </div>
        </div>

        <footer class="bottom-panel">
          <div class="summary-strip">
            <div class="summary-cell"><span>SSID</span><strong id="ssidValue">--</strong></div>
            <div class="summary-cell"><span>IP</span><strong id="ipValue">0.0.0.0</strong></div>
            <div class="summary-cell"><span>Transport</span><strong id="videoTransport">reserved</strong></div>
            <div class="summary-cell"><span>Source</span><strong id="videoSource">not attached</strong></div>
          </div>
          <div class="motor-strip">
            <div class="motor-chip"><span>M1</span><strong id="m1Value">0.00</strong></div>
            <div class="motor-chip"><span>M2</span><strong id="m2Value">0.00</strong></div>
            <div class="motor-chip"><span>M3</span><strong id="m3Value">0.00</strong></div>
            <div class="motor-chip"><span>M4</span><strong id="m4Value">0.00</strong></div>
          </div>
        </footer>
      </div>
    </div>

    <div class="menu-backdrop" id="menuBackdrop"></div>

    <aside class="menu-drawer" id="menuDrawer">
      <div class="menu-head">
        <div class="menu-title-wrap">
          <div class="menu-title">Flight Menu</div>
          <div class="menu-subtitle">Fixed single-screen cockpit. Open settings from here without changing the live layout.</div>
        </div>
        <button class="close-button" id="closeMenuBtn">X</button>
      </div>

      <div class="menu-tabs">
        <button class="menu-tab active" data-panel="system">System</button>
        <button class="menu-tab" data-panel="video">Video</button>
        <button class="menu-tab" data-panel="params">Params</button>
        <button class="menu-tab" data-panel="wave">Wave</button>
      </div>

      <div class="menu-panels">
        <section class="menu-panel active" data-panel="system">
          <div class="menu-scroll">
            <div class="menu-card">
              <h3>Connection</h3>
              <div class="info-list">
                <div class="info-row"><span>mDNS Host</span><strong id="menuHostValue">madflight.local</strong></div>
                <div class="info-row"><span>Aircraft IP</span><strong id="menuIpValue">0.0.0.0</strong></div>
                <div class="info-row"><span>Preferred URL</span><strong id="menuPreferredUrl">http://0.0.0.0/</strong></div>
              </div>
              <p id="hotspotNote">Use the aircraft IP first when the phone hotspot does not resolve .local names.</p>
            </div>

            <div class="menu-card">
              <h3>Quick Access</h3>
              <div class="quick-grid">
                <button class="quick-tile" data-open-panel="video"><span>Menu</span><strong>Video Hook</strong></button>
                <button class="quick-tile" data-open-panel="params"><span>Menu</span><strong>Parameter Tuning</strong></button>
                <button class="quick-tile" data-open-panel="wave"><span>Menu</span><strong>Wave Monitor</strong></button>
                <button class="quick-tile" id="quickSaveBtn"><span>Storage</span><strong>Save To Flash</strong></button>
              </div>
            </div>

            <div class="menu-card">
              <h3>Sensor Snapshot</h3>
              <div class="info-list">
                <div class="info-row"><span>IMU</span><strong id="menuImuValue">--</strong></div>
                <div class="info-row"><span>Barometer</span><strong id="menuBarValue">--</strong></div>
                <div class="info-row"><span>Magnetometer</span><strong id="menuMagValue">--</strong></div>
                <div class="info-row"><span>Motion</span><strong id="menuGyroValue">0 / 0 / 0</strong></div>
              </div>
            </div>
          </div>
        </section>

        <section class="menu-panel" data-panel="video">
          <div class="menu-scroll">
            <div class="menu-card">
              <h3>Video Transport</h3>
              <div class="info-list">
                <div class="info-row"><span>Status</span><strong id="menuCameraState">Reserved</strong></div>
                <div class="info-row"><span>Transport</span><strong id="menuVideoTransport">reserved</strong></div>
                <div class="info-row"><span>Source URL</span><strong id="menuVideoSource">not attached</strong></div>
              </div>
              <p>The cockpit is already shaped for video-first use. When the camera pipeline is ready, inject it into the big live area through the built-in hook API.</p>
              <div class="drawer-foot">
                <button class="btn secondary" id="openVideoHookBtn">Show Hook API</button>
                <button class="btn warn" id="clearVideoBtn">Clear Source</button>
              </div>
            </div>

            <div class="menu-card">
              <h3>Hook API</h3>
              <div class="info-list">
                <div class="info-row"><span>Attach</span><strong>MadflightUI.attachVideoSource()</strong></div>
                <div class="info-row"><span>Clear</span><strong>MadflightUI.clearVideo()</strong></div>
              </div>
              <p>The current placeholder note inside the viewport can also be updated with <strong>MadflightUI.setVideoNote()</strong>.</p>
            </div>
          </div>
        </section>

        <section class="menu-panel" data-panel="params">
          <input class="searchbar" id="paramSearch" placeholder="Filter params, for example: usr_ / imu / bar / rcl">
          <div class="param-list" id="paramList"></div>
          <div class="drawer-foot">
            <button class="btn secondary" id="reloadParamsBtn">Reload</button>
            <button class="btn" id="saveParamsBtn">Save To Flash</button>
          </div>
        </section>

        <section class="menu-panel" data-panel="wave">
          <div class="wave-wrap">
            <canvas id="waveCanvas"></canvas>
            <div class="wave-legend" id="waveLegend"></div>
          </div>
          <div class="drawer-foot">
            <button class="btn secondary" id="pauseWaveBtn">Pause</button>
            <button class="btn secondary" id="refreshWaveBtn">Refresh Now</button>
            <button class="btn" id="fitWaveBtn">Fit View</button>
          </div>
        </section>
      </div>
    </aside>

    <div class="toast" id="toast"></div>
  </div>

  <script>
    (() => {
      const state = {
        overview: null,
        params: null,
        wave: null,
        wavePaused: false,
        fitWave: true,
        videoSource: '',
        videoTransport: 'reserved',
        paramsLoaded: false,
        menuOpen: false,
        activePanel: 'system',
      };

      const els = {};
      const q = (id) => document.getElementById(id);

      async function fetchJson(url, options) {
        const response = await fetch(url, options);
        if (!response.ok) {
          throw new Error(`${response.status} ${response.statusText}`);
        }
        return response.json();
      }

      function setText(id, value) {
        const el = els[id] || q(id);
        if (!el) return;
        el.textContent = value;
      }

      function fmt(num, digits = 2) {
        const n = Number(num || 0);
        return Number.isFinite(n) ? n.toFixed(digits) : '--';
      }

      function toast(message, timeout = 1800) {
        const el = els.toast;
        el.textContent = message;
        el.classList.add('show');
        window.clearTimeout(toast._timer);
        toast._timer = window.setTimeout(() => el.classList.remove('show'), timeout);
      }

      function openMenu(panel = state.activePanel) {
        state.menuOpen = true;
        document.body.classList.add('menu-open');
        els.menuDrawer.classList.add('open');
        els.menuBackdrop.classList.add('open');
        switchMenuPanel(panel);
      }

      function closeMenu() {
        state.menuOpen = false;
        document.body.classList.remove('menu-open');
        els.menuDrawer.classList.remove('open');
        els.menuBackdrop.classList.remove('open');
      }

      function switchMenuPanel(panel) {
        state.activePanel = panel;

        document.querySelectorAll('.menu-tab').forEach((tab) => {
          tab.classList.toggle('active', tab.dataset.panel === panel);
        });

        document.querySelectorAll('.menu-panel').forEach((section) => {
          section.classList.toggle('active', section.dataset.panel === panel);
        });

        if (panel === 'params') ensureParams();
        if (panel === 'wave') refreshWave();
      }

      function bindButtons() {
        els.openMenuBtn.addEventListener('click', () => openMenu('system'));
        els.closeMenuBtn.addEventListener('click', closeMenu);
        els.menuBackdrop.addEventListener('click', closeMenu);

        document.querySelectorAll('.menu-tab').forEach((tab) => {
          tab.addEventListener('click', () => switchMenuPanel(tab.dataset.panel));
        });

        document.querySelectorAll('[data-open-panel]').forEach((btn) => {
          btn.addEventListener('click', () => openMenu(btn.dataset.openPanel));
        });

        document.addEventListener('keydown', (event) => {
          if (event.key === 'Escape') closeMenu();
        });

        els.openVideoHookBtn.addEventListener('click', async () => {
          try {
            const info = await fetchJson('/api/video/config');
            toast(`Hook ready: ${info.attach_api}`);
          } catch (error) {
            toast(`Video hook error: ${error.message}`, 2600);
          }
        });

        els.clearVideoBtn.addEventListener('click', () => {
          window.MadflightUI.clearVideo();
          toast('Video source cleared');
        });

        els.reloadParamsBtn.addEventListener('click', () => ensureParams(true));
        els.saveParamsBtn.addEventListener('click', saveParams);
        els.quickSaveBtn.addEventListener('click', saveParams);
        els.paramSearch.addEventListener('input', renderParams);

        els.pauseWaveBtn.addEventListener('click', () => {
          state.wavePaused = !state.wavePaused;
          els.pauseWaveBtn.textContent = state.wavePaused ? 'Resume' : 'Pause';
        });

        els.refreshWaveBtn.addEventListener('click', refreshWave);
        els.fitWaveBtn.addEventListener('click', () => {
          state.fitWave = !state.fitWave;
          els.fitWaveBtn.textContent = state.fitWave ? 'Fit View' : 'Fixed View';
          if (state.wave) drawWave(state.wave);
        });
      }

      function updateCompass(yawDeg) {
        const yaw = Number(yawDeg || 0);
        els.compassNeedle.style.transform = `translateX(-50%) rotate(${yaw}deg)`;
        setText('yawCompassValue', `${fmt(yaw, 1)} deg`);
      }

      function renderOverview(data) {
        const wifiOk = !!data.wifi_connected;
        const preferredUrl = data.preferred_url || (data.ip ? `http://${data.ip}/` : 'http://madflight.local/');

        els.wifiDot.classList.toggle('ok', wifiOk);
        setText('wifiState', wifiOk ? 'Online' : 'Offline');
        setText('armValue', data.flight.armed ? 'ARMED' : 'DISARMED');
        setText('rcValue', data.flight.rcl_connected ? 'Connected' : 'Offline');
        setText('footerMessage', wifiOk ? `Open ${preferredUrl}` : 'Waiting for hotspot link');

        setText('thrValue', fmt(data.flight.throttle, 2));
        setText('rateValue', `${fmt(data.imu.sample_rate_hz, 0)} Hz`);
        setText('footerLink', wifiOk ? 'GOOD' : 'WAIT');
        setText('footerVideo', data.video.stream_url ? 'LIVE' : 'READY');

        setText('domainValue', data.hostname_mdns || data.hostname || 'madflight.local');
        setText('urlValue', preferredUrl);
        setText('rollValue', `${fmt(data.ahr.roll_deg, 1)} deg`);
        setText('pitchValue', `${fmt(data.ahr.pitch_deg, 1)} deg`);
        setText('yawValue', `${fmt(data.ahr.yaw_deg, 1)} deg`);
        setText('imuName', data.imu.name || '--');
        setText('barName', data.bar.name || '--');
        setText('magName', data.mag.name || '--');

        setText('gyroValue', `${fmt(data.imu.gx, 1)} / ${fmt(data.imu.gy, 1)} / ${fmt(data.imu.gz, 1)}`);
        setText('accelValue', `${fmt(data.imu.ax, 2)} / ${fmt(data.imu.ay, 2)} / ${fmt(data.imu.az, 2)}`);

        setText('altValue', `${fmt(data.bar.alt_m, 2)} m`);
        setText('pressValue', `${fmt(data.bar.press_pa, 1)} Pa`);
        setText('footerArm', data.flight.armed ? 'ARMED' : 'DISARMED');
        setText('cameraState', data.video.transport || 'reserved');
        setText('footerSave', els.footerSave.textContent || 'IDLE');
        setText('waveState', state.wave && !state.wavePaused ? 'Streaming' : 'Ready');

        setText('ssidValue', data.wifi_ssid || '--');
        setText('ipValue', data.ip || '0.0.0.0');
        setText('videoTransport', data.video.transport || 'reserved');
        setText('videoSource', data.video.stream_url || 'not attached');

        setText('m1Value', fmt(data.outputs.m1, 2));
        setText('m2Value', fmt(data.outputs.m2, 2));
        setText('m3Value', fmt(data.outputs.m3, 2));
        setText('m4Value', fmt(data.outputs.m4, 2));

        setText('modeValue', data.flight.mode || 'RATE');
        setText('magValue', `${fmt(data.mag.mx_ut, 1)} / ${fmt(data.mag.my_ut, 1)} / ${fmt(data.mag.mz_ut, 1)}`);
        updateCompass(data.ahr.yaw_deg);

        setText('menuHostValue', data.hostname_mdns || data.hostname || 'madflight.local');
        setText('menuIpValue', data.ip || '0.0.0.0');
        setText('menuPreferredUrl', preferredUrl);
        setText('hotspotNote', data.hotspot_note || 'Use the aircraft IP first when the hotspot does not resolve .local.');
        setText('menuImuValue', data.imu.name || '--');
        setText('menuBarValue', data.bar.name || '--');
        setText('menuMagValue', data.mag.name || '--');
        setText('menuGyroValue', `${fmt(data.imu.gx, 1)} / ${fmt(data.imu.gy, 1)} / ${fmt(data.imu.gz, 1)}`);
        setText('menuCameraState', data.video.transport || 'reserved');
        setText('menuVideoTransport', data.video.transport || 'reserved');
        setText('menuVideoSource', data.video.stream_url || 'not attached');

        if (!state.videoSource && data.video.stream_url) {
          window.MadflightUI.attachVideoSource(data.video.stream_url, data.video.transport || 'img');
        }
      }

      async function refreshOverview() {
        try {
          const data = await fetchJson('/api/overview');
          state.overview = data;
          renderOverview(data);
        } catch (error) {
          setText('wifiState', 'Error');
          toast(`Overview error: ${error.message}`, 2200);
        }
      }

      async function ensureParams(force = false) {
        if (state.paramsLoaded && !force) {
          renderParams();
          return;
        }

        try {
          const data = await fetchJson('/api/params');
          state.params = data;
          state.paramsLoaded = true;
          renderParams();
          toast(`Loaded ${data.items.length} params`);
        } catch (error) {
          toast(`Param load failed: ${error.message}`, 2600);
        }
      }

      function renderParams() {
        const holder = els.paramList;
        holder.innerHTML = '';

        if (!state.params || !Array.isArray(state.params.items)) return;

        const filter = (els.paramSearch.value || '').trim().toLowerCase();
        const items = state.params.items.filter((item) => {
          if (!filter) return true;
          return item.name.toLowerCase().includes(filter) || item.scope.toLowerCase().includes(filter);
        });

        items.forEach((item) => {
          const row = document.createElement('div');
          row.className = 'param-row';

          const meta = document.createElement('div');
          meta.className = 'param-name';
          meta.innerHTML = `<strong>${item.name}</strong><span>${item.scope}</span>`;

          const input = document.createElement('input');
          input.value = item.value;
          input.spellcheck = false;

          const button = document.createElement('button');
          button.className = 'btn secondary';
          button.textContent = 'Apply';
          button.addEventListener('click', () => applyParam(item.scope, item.name, input.value));

          row.appendChild(meta);
          row.appendChild(input);
          row.appendChild(button);
          holder.appendChild(row);
        });
      }

      async function applyParam(scope, name, value) {
        const form = new URLSearchParams();
        form.set('scope', scope);
        form.set('name', name);
        form.set('value', value);

        try {
          const data = await fetchJson('/api/param/set', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8' },
            body: form.toString(),
          });

          if (!data.ok) {
            toast(`Apply failed: ${name}`);
            return;
          }

          setText('footerSave', 'DIRTY');
          toast(`Applied ${name}`);
        } catch (error) {
          toast(`Apply error: ${error.message}`, 2600);
        }
      }

      async function saveParams() {
        try {
          const data = await fetchJson('/api/param/save', { method: 'POST' });
          if (data.ok) {
            setText('footerSave', 'SAVED');
            toast('Parameters saved to flash');
          } else {
            toast('Save failed', 2200);
          }
        } catch (error) {
          toast(`Save error: ${error.message}`, 2600);
        }
      }

      function drawWave(payload) {
        const canvas = els.waveCanvas;
        const ctx = canvas.getContext('2d');
        const dpr = window.devicePixelRatio || 1;
        const width = canvas.clientWidth;
        const height = canvas.clientHeight;

        canvas.width = Math.max(1, Math.floor(width * dpr));
        canvas.height = Math.max(1, Math.floor(height * dpr));
        ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

        ctx.clearRect(0, 0, width, height);
        ctx.fillStyle = 'rgba(255,255,255,0.02)';
        ctx.fillRect(0, 0, width, height);

        ctx.strokeStyle = 'rgba(255,255,255,0.08)';
        ctx.lineWidth = 1;
        for (let i = 0; i < 5; i += 1) {
          const y = (height / 4) * i;
          ctx.beginPath();
          ctx.moveTo(0, y);
          ctx.lineTo(width, y);
          ctx.stroke();
        }

        const channels = payload.channels || [];
        let min = -1;
        let max = 1;

        if (state.fitWave) {
          min = Infinity;
          max = -Infinity;
          channels.forEach((channel) => {
            (channel.samples || []).forEach((sample) => {
              if (sample < min) min = sample;
              if (sample > max) max = sample;
            });
          });

          if (!Number.isFinite(min) || !Number.isFinite(max) || min === max) {
            min = -1;
            max = 1;
          } else {
            const pad = Math.max(0.05, (max - min) * 0.15);
            min -= pad;
            max += pad;
          }
        }

        channels.forEach((channel) => {
          const samples = channel.samples || [];
          if (samples.length < 2) return;

          ctx.beginPath();
          ctx.strokeStyle = channel.color || '#ffffff';
          ctx.lineWidth = 2;

          samples.forEach((sample, index) => {
            const x = (width * index) / Math.max(1, samples.length - 1);
            const y = height - ((sample - min) / Math.max(0.0001, max - min)) * height;
            if (index === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
          });

          ctx.stroke();
        });

        els.waveLegend.innerHTML = '';
        channels.forEach((channel) => {
          const tag = document.createElement('div');
          tag.className = 'wave-tag';
          tag.innerHTML = `<span class="wave-color" style="background:${channel.color || '#fff'}"></span>${channel.name}`;
          els.waveLegend.appendChild(tag);
        });
      }

      async function refreshWave() {
        if (!state.menuOpen || state.activePanel !== 'wave' || state.wavePaused) return;

        try {
          const data = await fetchJson('/api/waveform');
          state.wave = data;
          drawWave(data);
          setText('waveState', 'Streaming');
        } catch (error) {
          setText('waveState', 'Error');
        }
      }

      function installVideoHooks() {
        window.MadflightUI = {
          attachVideoSource(url, type = 'img') {
            state.videoSource = url || '';
            state.videoTransport = type || 'img';

            const feed = els.videoFeed;
            const existing = feed.querySelector('.attached-video');
            if (existing) existing.remove();

            if (!url) {
              els.videoPlaceholder.style.display = '';
              setText('videoSource', 'not attached');
              setText('menuVideoSource', 'not attached');
              setText('videoTransport', 'reserved');
              setText('menuVideoTransport', 'reserved');
              setText('footerVideo', 'READY');
              return;
            }

            let node;
            if (type === 'iframe') {
              node = document.createElement('iframe');
              node.allow = 'autoplay; fullscreen';
            } else if (type === 'video') {
              node = document.createElement('video');
              node.autoplay = true;
              node.muted = true;
              node.playsInline = true;
              node.controls = false;
            } else {
              node = document.createElement('img');
            }

            node.className = 'attached-video';
            node.src = url;
            feed.appendChild(node);
            els.videoPlaceholder.style.display = 'none';

            setText('videoSource', url);
            setText('menuVideoSource', url);
            setText('videoTransport', type);
            setText('menuVideoTransport', type);
            setText('footerVideo', 'HOOKED');
            setText('cameraState', 'Live');
            setText('menuCameraState', 'Live');
          },

          clearVideo() {
            state.videoSource = '';
            const existing = els.videoFeed.querySelector('.attached-video');
            if (existing) existing.remove();
            els.videoPlaceholder.style.display = '';
            setText('videoSource', 'not attached');
            setText('menuVideoSource', 'not attached');
            setText('videoTransport', 'reserved');
            setText('menuVideoTransport', 'reserved');
            setText('footerVideo', 'READY');
            setText('cameraState', 'Reserved');
            setText('menuCameraState', 'Reserved');
          },

          setVideoNote(note) {
            setText('videoNote', note);
          },
        };
      }

      function cacheElements() {
        [
          'videoFeed', 'videoPlaceholder', 'videoNote', 'toast',
          'wifiDot', 'wifiState', 'armValue', 'rcValue', 'openMenuBtn',
          'footerMessage', 'thrValue', 'rateValue', 'footerLink', 'footerVideo',
          'domainValue', 'urlValue', 'rollValue', 'pitchValue', 'yawValue',
          'imuName', 'barName', 'magName', 'gyroValue', 'accelValue',
          'altValue', 'pressValue', 'footerArm', 'cameraState', 'footerSave',
          'waveState', 'yawCompassValue', 'compassNeedle', 'magValue',
          'ssidValue', 'ipValue', 'videoTransport', 'videoSource',
          'm1Value', 'm2Value', 'm3Value', 'm4Value', 'modeValue',
          'menuBackdrop', 'menuDrawer', 'closeMenuBtn',
          'menuHostValue', 'menuIpValue', 'menuPreferredUrl', 'hotspotNote',
          'menuImuValue', 'menuBarValue', 'menuMagValue', 'menuGyroValue',
          'menuCameraState', 'menuVideoTransport', 'menuVideoSource',
          'openVideoHookBtn', 'clearVideoBtn',
          'paramSearch', 'paramList', 'reloadParamsBtn', 'saveParamsBtn', 'quickSaveBtn',
          'pauseWaveBtn', 'refreshWaveBtn', 'fitWaveBtn', 'waveCanvas', 'waveLegend'
        ].forEach((id) => {
          els[id] = q(id);
        });
      }

      async function boot() {
        cacheElements();
        bindButtons();
        installVideoHooks();
        await refreshOverview();
        window.setInterval(refreshOverview, 650);
        window.setInterval(refreshWave, 280);
      }

      window.addEventListener('DOMContentLoaded', boot);
    })();
  </script>
</body>
</html>
)USRUI";
