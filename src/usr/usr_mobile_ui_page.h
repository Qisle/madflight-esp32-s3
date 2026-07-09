#pragma once

#include <Arduino.h>

static const char USR_MOBILE_UI_PAGE[] PROGMEM = R"USRUI(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover, maximum-scale=1, user-scalable=0">
  <meta name="screen-orientation" content="landscape">
  <meta name="x5-orientation" content="landscape">
  <title>Madflight Cockpit</title>
  <style>
    :root {
      --bg: #000;
      --text: #fff;
      --muted: rgba(255, 255, 255, 0.6);
      --green: #30e4c5;
      --red: #ff4d4d;
      --glass: rgba(0, 0, 0, 0.5);
      --glass-border: rgba(255, 255, 255, 0.15);
      --font-ui: "Segoe UI", -apple-system, sans-serif;
      --safe-top: env(safe-area-inset-top, 0px);
      --safe-right: env(safe-area-inset-right, 0px);
      --safe-bottom: env(safe-area-inset-bottom, 0px);
      --safe-left: env(safe-area-inset-left, 0px);
      --vw: 100vw;
      --vh: 100vh;
      --ui-width: 100vw;
      --ui-height: 100vh;
      --ui-short: 100vh;
      --ui-long: 100vw;
      --edge-gap: clamp(10px, calc(var(--ui-short) * 0.03), 20px);
      --cluster-gap: clamp(8px, calc(var(--ui-short) * 0.032), 16px);
      --badge-gap: clamp(4px, calc(var(--ui-short) * 0.014), 6px);
      --badge-pad-y: clamp(8px, calc(var(--ui-short) * 0.022), 11px);
      --badge-pad-x: clamp(12px, calc(var(--ui-short) * 0.034), 16px);
      --topbar-font: clamp(12px, calc(var(--ui-short) * 0.034), 15px);
      --label-font: clamp(9px, calc(var(--ui-short) * 0.024), 11px);
      --value-font: clamp(13px, calc(var(--ui-short) * 0.035), 15px);
      --topbar-height: clamp(46px, calc(var(--ui-short) * 0.13), 58px);
      --menu-btn-size: clamp(42px, calc(var(--ui-short) * 0.12), 50px);
      --menu-dot-size: clamp(3px, calc(var(--ui-short) * 0.01), 4px);
      --placeholder-width: min(clamp(220px, calc(var(--ui-width) * 0.62), 640px), calc(var(--ui-height) * 1.72));
      --placeholder-padding: clamp(14px, calc(var(--ui-short) * 0.042), 18px);
      --placeholder-radius: clamp(14px, calc(var(--ui-short) * 0.045), 18px);
      --placeholder-title-size: clamp(16px, calc(var(--ui-short) * 0.042), 18px);
      --placeholder-note-size: clamp(11px, calc(var(--ui-short) * 0.03), 13px);
      --reticle-width: clamp(102px, calc(var(--ui-short) * 0.285), 120px);
      --reticle-height: clamp(76px, calc(var(--ui-short) * 0.215), 90px);
      --reticle-tick: clamp(8px, calc(var(--ui-short) * 0.024), 10px);
      --telemetry-offset: clamp(12px, calc(var(--ui-short) * 0.05), 20px);
      --telemetry-gap: clamp(8px, calc(var(--ui-short) * 0.032), 18px);
      --telemetry-pad-y: clamp(5px, calc(var(--ui-short) * 0.014), 6px);
      --telemetry-pad-x: clamp(14px, calc(var(--ui-short) * 0.045), 20px);
      --telemetry-radius: clamp(16px, calc(var(--ui-short) * 0.05), 20px);
      --telemetry-max-width: calc(var(--ui-width) - clamp(132px, calc(var(--ui-short) * 0.35), 196px) - var(--safe-left) - var(--safe-right));
      --radar-size: clamp(58px, calc(var(--ui-short) * 0.17), 78px);
      --radar-center-size: clamp(5px, calc(var(--ui-short) * 0.015), 6px);
      --radar-arrow-top: clamp(7px, calc(var(--ui-short) * 0.019), 8px);
      --radar-arrow-half: clamp(4px, calc(var(--ui-short) * 0.012), 5px);
      --radar-arrow-height: clamp(8px, calc(var(--ui-short) * 0.024), 10px);
      --radar-arrow-origin: calc(var(--radar-size) * 0.413);
      --stick-size: clamp(128px, calc(var(--ui-short) * 0.36), 168px);
      --stick-knob-size: clamp(44px, calc(var(--ui-short) * 0.13), 58px);
      --stick-ring-size: calc(var(--stick-size) - clamp(12px, calc(var(--ui-short) * 0.03), 16px));
      --stick-bottom: calc(var(--telemetry-offset) + var(--safe-bottom) + clamp(72px, calc(var(--ui-short) * 0.18), 92px));
      --stick-side-gap: clamp(16px, calc(var(--ui-short) * 0.05), 26px);
      --control-pill-width: clamp(112px, calc(var(--ui-short) * 0.3), 140px);
      --arm-btn-height: clamp(52px, calc(var(--ui-short) * 0.135), 64px);
      --arm-btn-font: clamp(13px, calc(var(--ui-short) * 0.035), 15px);
      --stick-readout-font: clamp(10px, calc(var(--ui-short) * 0.024), 11px);
      --tool-gap: clamp(10px, calc(var(--ui-short) * 0.035), 15px);
      --tool-size: clamp(48px, calc(var(--ui-short) * 0.13), 56px);
      --tool-icon-size: clamp(18px, calc(var(--ui-short) * 0.05), 22px);
      --shutter-size: clamp(56px, calc(var(--ui-short) * 0.142), 60px);
      --shutter-inner-size: clamp(40px, calc(var(--ui-short) * 0.105), 44px);
      --hud-offset: clamp(126px, calc(var(--ui-short) * 0.35), 150px);
      --hud-line-width: clamp(24px, calc(var(--ui-short) * 0.072), 30px);
      --hud-line-gap: clamp(16px, calc(var(--ui-short) * 0.047), 20px);
      --hud-font: clamp(10px, calc(var(--ui-short) * 0.024), 11px);
      --hud-label-offset: clamp(21px, calc(var(--ui-short) * 0.06), 25px);
      --drawer-width: min(clamp(280px, calc(var(--ui-width) * 0.34), 340px), calc(var(--ui-width) - var(--safe-left) - var(--safe-right)));
      --drawer-pad-x: clamp(16px, calc(var(--ui-short) * 0.05), 20px);
      --drawer-pad-y: clamp(12px, calc(var(--ui-short) * 0.038), 15px);
      --panel-font: clamp(12px, calc(var(--ui-short) * 0.031), 13px);
      --panel-title-font: clamp(15px, calc(var(--ui-short) * 0.04), 16px);
      --button-font: clamp(13px, calc(var(--ui-short) * 0.035), 15px);
      --button-pad-y: clamp(9px, calc(var(--ui-short) * 0.026), 12px);
      --button-pad-x: clamp(14px, calc(var(--ui-short) * 0.042), 18px);
      --input-pad: clamp(6px, calc(var(--ui-short) * 0.016), 8px);
      --input-width: clamp(56px, calc(var(--ui-short) * 0.14), 60px);
      --wave-height: clamp(120px, calc(var(--ui-short) * 0.34), 140px);
      --toast-top: calc(var(--topbar-height) + var(--safe-top) + 16px);
    }
    * { box-sizing: border-box; }
    html, body { width: var(--vw); height: var(--vh); margin: 0; overflow: hidden; }
    html { background: var(--bg); }
    body { background: var(--bg); color: var(--text); font-family: var(--font-ui); touch-action: none; user-select: none; -webkit-user-select: none; position: fixed; inset: 0; overscroll-behavior: none; }
    body.menu-open { overflow: hidden; }
    button, input { font: inherit; outline: none; }
    
    .video-stage { position: absolute; inset: 0; width: 100%; height: 100%; background: #000; z-index: 1; }
    .video-feed { position: absolute; inset: 0; display: grid; place-items: center; overflow: hidden; }
    .video-feed img, .video-feed video, .video-feed iframe { width: 100%; height: 100%; object-fit: cover; }
    #videoPlaceholder { width: var(--placeholder-width); aspect-ratio: 16/9; border: 1px dashed rgba(255,255,255,0.12); border-radius: var(--placeholder-radius); display: grid; place-items: center; text-align: center; background: radial-gradient(circle at center, rgba(255,255,255,0.04) 0%, rgba(255,255,255,0.02) 55%, rgba(255,255,255,0.01) 100%); padding: var(--placeholder-padding); }
    .placeholder-title { display: block; font-size: var(--placeholder-title-size); margin-bottom: calc(var(--placeholder-padding) * 0.45); }
    .placeholder-note { color: var(--muted); font-size: var(--placeholder-note-size); }
    
    /* Top Status Bar */
    .top-bar { position: absolute; top: 0; left: 0; right: 0; height: calc(var(--topbar-height) + var(--safe-top)); display: flex; justify-content: space-between; align-items: center; padding: var(--safe-top) calc(var(--edge-gap) + var(--safe-right)) 0 calc(var(--edge-gap) + var(--safe-left)); font-size: var(--topbar-font); text-shadow: 0 1px 3px rgba(0,0,0,0.8); z-index: 10; pointer-events: none; background: linear-gradient(180deg, rgba(0,0,0,0.6) 0%, transparent 100%); }
    .top-left, .top-center, .top-right { display: flex; align-items: center; gap: var(--cluster-gap); }
    .top-right { pointer-events: auto; }
    .mode-anchor { position: relative; pointer-events: auto; }
    .top-logo { height: clamp(12px, calc(var(--ui-short) * 0.038), 16px); margin-right: calc(var(--cluster-gap) * 0.6); opacity: 0.95; }
    .icon-label { font-size: var(--label-font); color: var(--muted); margin-right: calc(var(--badge-gap) * 0.66); font-weight: 600; }
    .status-badge { min-height: 42px; display: flex; align-items: center; gap: var(--badge-gap); background: rgba(0,0,0,0.4); padding: var(--badge-pad-y) var(--badge-pad-x); border-radius: calc(var(--topbar-height) * 0.36); backdrop-filter: blur(4px); -webkit-backdrop-filter: blur(4px); border: 1px solid rgba(255,255,255,0.1); }
    .status-badge.tap-badge { pointer-events: auto; cursor: pointer; }
    .status-badge.tap-badge:active { background: rgba(255,255,255,0.16); }
    .status-badge.tap-badge.open { background: rgba(48,228,197,0.16); border-color: rgba(48,228,197,0.28); }
    #armValue { color: var(--red); font-weight: bold; }
    .arm-button { min-width: clamp(102px, calc(var(--ui-short) * 0.28), 130px); justify-content: center; background: rgba(255,77,77,0.15); border-color: rgba(255,77,77,0.3); color: #fff; }
    .arm-button.armed { background: rgba(48,228,197,0.15); border-color: rgba(48,228,197,0.32); }
    .arm-button.armed #armValue { color: var(--green); }
    .arm-button.holding { box-shadow: inset 0 -3px 0 rgba(48,228,197,0.86), 0 0 0 1px rgba(48,228,197,0.2); }
    .mode-menu { position: absolute; left: 0; top: calc(100% + 8px); min-width: clamp(132px, calc(var(--ui-short) * 0.34), 168px); padding: 8px; border-radius: 16px; background: rgba(10, 14, 18, 0.92); border: 1px solid rgba(255,255,255,0.12); box-shadow: 0 18px 42px rgba(0,0,0,0.36); backdrop-filter: blur(14px); -webkit-backdrop-filter: blur(14px); z-index: 120; opacity: 0; pointer-events: none; transform: translateY(-4px) scale(0.98); transition: opacity 0.18s ease, transform 0.18s ease; }
    .mode-menu.open { opacity: 1; pointer-events: auto; transform: translateY(0) scale(1); }
    .mode-option { width: 100%; display: flex; flex-direction: column; align-items: flex-start; gap: 2px; padding: 10px 12px; border: none; border-radius: 12px; background: transparent; color: #fff; font-size: var(--button-font); text-align: left; cursor: pointer; }
    .mode-option:active { background: rgba(255,255,255,0.12); }
    .mode-option.active { background: rgba(48,228,197,0.16); box-shadow: inset 0 0 0 1px rgba(48,228,197,0.24); }
    .mode-option small { color: var(--muted); font-size: var(--label-font); letter-spacing: 0.08em; }
    
    .menu-btn { cursor: pointer; background: rgba(0,0,0,0.4); border: 1px solid rgba(255,255,255,0.2); border-radius: 50%; width: var(--menu-btn-size); height: var(--menu-btn-size); display: grid; place-items: center; backdrop-filter: blur(4px); transition: background 0.2s; }
    .menu-btn:active { background: rgba(255,255,255,0.2); }
    .menu-btn div { display: flex; gap: calc(var(--menu-dot-size) * 0.75); }
    .menu-btn div span { display: block; width: var(--menu-dot-size); height: var(--menu-dot-size); background: #fff; border-radius: 50%; }
    .status-dot { width: clamp(5px, calc(var(--ui-short) * 0.015), 6px); height: clamp(5px, calc(var(--ui-short) * 0.015), 6px); background: var(--red); border-radius: 50%; }

    /* Center Reticle */
    .center-reticle { position: absolute; inset: 0; display: grid; place-items: center; pointer-events: none; z-index: 5; }
    .af-box { width: var(--reticle-width); height: var(--reticle-height); border: 1px solid rgba(48, 228, 197, 0.8); position: relative; }
    .af-box::before, .af-box::after { content: ""; position: absolute; background: rgba(48,228,197,0.8); }
    .af-box::before { width: var(--reticle-tick); height: 1px; top: 50%; left: 50%; transform: translate(-50%, -50%); }
    .af-box::after { width: 1px; height: var(--reticle-tick); top: 50%; left: 50%; transform: translate(-50%, -50%); }

    /* Telemetry Info - Bottom */
    .bottom-telemetry { position: absolute; bottom: calc(var(--telemetry-offset) + var(--safe-bottom)); left: 50%; transform: translateX(-50%); display: flex; gap: var(--telemetry-gap); z-index: 10; padding: var(--telemetry-pad-y) var(--telemetry-pad-x); background: rgba(0,0,0,0.4); border-radius: var(--telemetry-radius); backdrop-filter: blur(8px); border: 1px solid rgba(255,255,255,0.1); text-shadow: 0 1px 2px rgba(0,0,0,0.8); pointer-events: none; max-width: var(--telemetry-max-width); }
    .telemetry-item { display: flex; align-items: baseline; gap: var(--badge-gap); white-space: nowrap; flex-shrink: 0; }
    .telemetry-divider { width: 1px; background: rgba(255,255,255,0.2); margin: 0 calc(var(--telemetry-gap) * 0.25); }
    .t-label { font-size: var(--label-font); color: var(--muted); font-weight: 600; }
    .t-val { font-size: var(--value-font); font-weight: bold; color: #fff; line-height: 1; white-space: nowrap; }
    #accelValue, #gyroValue { font-size: clamp(11px, calc(var(--ui-short) * 0.031), 13px); letter-spacing: -0.02em; }
    
    /* Attitude Ball - Bottom Left */
    .radar { position: absolute; left: calc(var(--edge-gap) + var(--safe-left)); bottom: calc(var(--telemetry-offset) + var(--safe-bottom)); width: var(--radar-size); height: var(--radar-size); background: rgba(0,0,0,0.5); backdrop-filter: blur(8px); border-radius: 50%; border: 1px solid rgba(255,255,255,0.15); z-index: 10; pointer-events: none; display: grid; place-items: center; overflow: hidden; }
    .radar-inner { width: 100%; height: 100%; border-radius: 50%; position: relative; display: grid; place-items: center; border: 1px dashed rgba(255,255,255,0.1); background: radial-gradient(circle at 50% 40%, rgba(255,255,255,0.05) 0%, rgba(255,255,255,0.02) 48%, rgba(0,0,0,0.18) 100%); }
    .attitude-horizon { position: absolute; left: 50%; top: 50%; width: 176%; height: 176%; transform: translate(-50%, -50%); transition: transform 0.05s linear; }
    .attitude-horizon-roll { position: absolute; inset: 0; border-radius: 50%; overflow: hidden; transition: transform 0.05s linear; box-shadow: inset 0 0 14px rgba(0,0,0,0.28); }
    .attitude-sky { position: absolute; inset: 0 0 50% 0; background: linear-gradient(180deg, #6ca8d8 0%, #4b7fad 100%); }
    .attitude-ground { position: absolute; inset: 50% 0 0 0; background: linear-gradient(180deg, #7b4f33 0%, #503220 100%); }
    .attitude-line { position: absolute; left: -10%; right: -10%; top: calc(50% - 1px); height: 2px; background: rgba(255,255,255,0.88); box-shadow: 0 0 8px rgba(255,255,255,0.22); }
    .attitude-ladder { position: absolute; left: 50%; width: calc(var(--radar-size) * 0.34); border-top: 1px solid rgba(255,255,255,0.42); transform: translateX(-50%); }
    .attitude-ladder-up { top: 38%; }
    .attitude-ladder-down { top: 62%; }
    .attitude-bank { position: absolute; top: var(--radar-arrow-top); left: 50%; width: 0; height: 0; transform: translateX(-50%); border-left: var(--radar-arrow-half) solid transparent; border-right: var(--radar-arrow-half) solid transparent; border-bottom: var(--radar-arrow-height) solid var(--red); filter: drop-shadow(0 0 4px rgba(255, 77, 77, 0.24)); z-index: 5; }
    .attitude-marker { position: absolute; left: 50%; top: 50%; width: calc(var(--radar-size) * 0.48); height: calc(var(--radar-size) * 0.16); transform: translate(-50%, -50%); z-index: 6; }
    .attitude-wing { position: absolute; top: 50%; width: 34%; height: 2px; margin-top: -1px; background: rgba(255,255,255,0.92); box-shadow: 0 0 6px rgba(255,255,255,0.18); }
    .attitude-wing.left { left: 0; }
    .attitude-wing.right { right: 0; }
    .attitude-center { position: absolute; left: 50%; top: 50%; width: calc(var(--radar-center-size) + 3px); height: calc(var(--radar-center-size) + 3px); border: 1.5px solid rgba(255,255,255,0.92); border-radius: 50%; background: rgba(0,0,0,0.18); transform: translate(-50%, -50%); }
    .attitude-north { position: absolute; top: calc(var(--radar-size) * 0.045); left: 50%; transform: translateX(-50%); z-index: 8; font-size: clamp(8px, calc(var(--ui-short) * 0.022), 10px); line-height: 1; letter-spacing: 0.08em; font-weight: 700; color: rgba(255,255,255,0.9); text-shadow: 0 0 6px rgba(0,0,0,0.35); }
    .attitude-heading { position: absolute; inset: 0; z-index: 8; transition: transform 0.05s linear; }
    .attitude-heading-arrow { position: absolute; left: 50%; top: calc(var(--radar-size) * 0.13); width: 0; height: 0; transform: translateX(-50%); border-left: calc(var(--radar-arrow-half) * 0.9) solid transparent; border-right: calc(var(--radar-arrow-half) * 0.9) solid transparent; border-bottom: calc(var(--radar-arrow-height) * 0.95) solid #f4f7fb; filter: drop-shadow(0 0 5px rgba(255,255,255,0.22)); }

    /* Side Tools - Right */
    .right-tools { position: absolute; right: calc(var(--edge-gap) + var(--safe-right)); top: 50%; transform: translateY(-50%); display: flex; flex-direction: column; gap: var(--tool-gap); z-index: 10; align-items: center; }
    .tool-btn { width: var(--tool-size); height: var(--tool-size); background: rgba(0,0,0,0.4); border: 1px solid rgba(255,255,255,0.2); border-radius: 50%; display: grid; place-items: center; backdrop-filter: blur(4px); cursor: pointer; color: #fff; font-size: var(--tool-icon-size); pointer-events: auto; }
    .tool-btn:active { background: rgba(255,255,255,0.2); }
    .tool-icon { width: clamp(16px, calc(var(--ui-short) * 0.042), 18px); height: clamp(16px, calc(var(--ui-short) * 0.042), 18px); }
    .shutter { width: var(--shutter-size); height: var(--shutter-size); border: 3px solid #fff; border-radius: 50%; background: transparent; display: grid; place-items: center; cursor: pointer; pointer-events: auto; }
    .shutter-inner { width: var(--shutter-inner-size); height: var(--shutter-inner-size); border-radius: 50%; background: var(--red); transition: transform 0.1s; }
    .shutter:active .shutter-inner { transform: scale(0.9); }
    .control-pill { width: var(--control-pill-width); min-height: var(--arm-btn-height); display: flex; flex-direction: column; justify-content: center; align-items: center; gap: 2px; padding: calc(var(--button-pad-y) * 0.8) calc(var(--button-pad-x) * 0.9); background: rgba(0,0,0,0.42); border: 1px solid rgba(255,255,255,0.15); border-radius: calc(var(--arm-btn-height) * 0.36); backdrop-filter: blur(10px); pointer-events: auto; text-align: center; }
    .control-pill-label { font-size: var(--label-font); color: var(--muted); letter-spacing: 0.08em; }
    .control-pill-value { font-size: var(--button-font); font-weight: 700; color: #fff; }
    .mode-switch { display: none; width: var(--control-pill-width); gap: 6px; padding: 5px; background: rgba(0,0,0,0.42); border-radius: 999px; border: 1px solid rgba(255,255,255,0.15); backdrop-filter: blur(10px); pointer-events: auto; }
    .mode-chip { flex: 1; min-height: clamp(34px, calc(var(--ui-short) * 0.08), 40px); border: none; border-radius: 999px; background: transparent; color: rgba(255,255,255,0.66); font-size: var(--button-font); font-weight: 700; letter-spacing: 0.04em; }
    .mode-chip.active { background: rgba(48,228,197,0.18); color: #fff; box-shadow: inset 0 0 0 1px rgba(48,228,197,0.24); }
    .joystick { position: absolute; bottom: var(--stick-bottom); width: var(--stick-size); height: var(--stick-size); z-index: 12; pointer-events: auto; touch-action: none; user-select: none; -webkit-user-select: none; }
    .joystick.left { left: max(0px, calc(var(--safe-left) - clamp(6px, calc(var(--ui-short) * 0.014), 10px))); }
    .joystick.right { right: calc(var(--edge-gap) + var(--safe-right)); }
    .joystick-base { position: absolute; inset: 0; border-radius: 50%; background: radial-gradient(circle at 50% 44%, rgba(255,255,255,0.1) 0%, rgba(255,255,255,0.04) 48%, rgba(0,0,0,0.42) 100%); border: 1px solid rgba(255,255,255,0.14); backdrop-filter: blur(10px); box-shadow: inset 0 0 20px rgba(0,0,0,0.24); }
    .joystick-ring { position: absolute; left: 50%; top: 50%; width: var(--stick-ring-size); height: var(--stick-ring-size); border-radius: 50%; border: 1px dashed rgba(255,255,255,0.14); transform: translate(-50%, -50%); }
    .joystick-cross::before, .joystick-cross::after { content: ""; position: absolute; left: 50%; top: 50%; background: rgba(255,255,255,0.08); transform: translate(-50%, -50%); }
    .joystick-cross::before { width: 62%; height: 1px; }
    .joystick-cross::after { width: 1px; height: 62%; }
    .joystick-knob { position: absolute; left: 50%; top: 50%; width: var(--stick-knob-size); height: var(--stick-knob-size); margin-left: calc(var(--stick-knob-size) * -0.5); margin-top: calc(var(--stick-knob-size) * -0.5); border-radius: 50%; background: radial-gradient(circle at 35% 30%, rgba(255,255,255,0.92) 0%, rgba(255,255,255,0.3) 18%, rgba(23, 33, 42, 0.96) 100%); border: 1px solid rgba(255,255,255,0.22); box-shadow: 0 14px 26px rgba(0,0,0,0.28); transition: transform 0.04s linear; }
    .joystick-title { position: absolute; top: calc(var(--stick-size) * -0.18); left: 50%; transform: translateX(-50%); font-size: var(--label-font); color: rgba(255,255,255,0.78); letter-spacing: 0.12em; font-weight: 700; text-shadow: 0 1px 3px rgba(0,0,0,0.35); }
    .joystick-readout { position: absolute; bottom: calc(var(--stick-size) * -0.2); left: 50%; transform: translateX(-50%); min-width: calc(var(--stick-size) * 0.92); text-align: center; font-size: var(--stick-readout-font); color: rgba(255,255,255,0.74); text-shadow: 0 1px 3px rgba(0,0,0,0.4); }

    /* Alt Scale / Pitch Scale */
    .HUD-scale { position: absolute; z-index: 5; pointer-events: none; opacity: 0.8; }
    .scale-left { left: calc(50% - var(--hud-offset)); top: 50%; transform: translateY(-50%); }
    .scale-right { right: calc(50% - var(--hud-offset)); top: 50%; transform: translateY(-50%); }
    .hud-line { width: var(--hud-line-width); height: 2px; background: rgba(255,255,255,0.5); margin: var(--hud-line-gap) 0; position: relative; }
    .scale-left .hud-line::after { content: attr(data-val); position: absolute; left: calc(var(--hud-label-offset) * -1); top: -7px; font-size: var(--hud-font); color: #fff; }
    .scale-right .hud-line::after { content: attr(data-val); position: absolute; right: calc(var(--hud-label-offset) * -1); top: -7px; font-size: var(--hud-font); color: #fff; }

    /* Drawer/Menu (Glass effect) */
    .menu-backdrop { position: fixed; inset: 0; background: rgba(0,0,0,0.2); z-index: 99; opacity: 0; pointer-events: none; transition: opacity 0.3s; }
    .menu-backdrop.open { opacity: 1; pointer-events: auto; }
    .menu-drawer { position: fixed; top: 0; right: calc(-1 * (var(--drawer-width) + var(--safe-right) + 20px)); width: var(--drawer-width); height: 100%; background: rgba(15, 20, 25, 0.7); backdrop-filter: blur(20px); -webkit-backdrop-filter: blur(20px); z-index: 100; transition: right 0.3s cubic-bezier(0.2, 0.8, 0.2, 1); display: flex; flex-direction: column; border-left: 1px solid rgba(255,255,255,0.1); box-shadow: -5px 0 20px rgba(0,0,0,0.5); padding-right: var(--safe-right); }
    .menu-drawer.open { right: 0; }
    
    .menu-header { padding: var(--drawer-pad-y) var(--drawer-pad-x); display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid rgba(255,255,255,0.1); }
    .menu-header h2 { margin: 0; font-size: var(--panel-title-font); font-weight: 600; letter-spacing: 0.5px; }
    .close-btn { background: none; border: none; color: #fff; font-size: clamp(22px, calc(var(--ui-short) * 0.065), 26px); cursor: pointer; padding: 0; line-height: 1; opacity: 0.7; }
    .close-btn:active { opacity: 1; }

    .menu-tabs { display: flex; gap: calc(var(--cluster-gap) * 0.66); padding: var(--drawer-pad-y) var(--drawer-pad-x); overflow-x: auto; scrollbar-width: none; }
    .menu-tabs::-webkit-scrollbar { display: none; }
    .menu-tab { background: transparent; border: none; color: var(--muted); font-size: var(--panel-font); font-weight: 500; cursor: pointer; padding: calc(var(--drawer-pad-y) * 0.4) 0; position: relative; white-space: nowrap; }
    .menu-tab.active { color: #fff; }
    .menu-tab.active::after { content: ""; position: absolute; bottom: 0; left: 0; width: 100%; height: 2px; background: var(--green); border-radius: 2px; }

    .menu-content { flex: 1; overflow-y: auto; padding: 0 var(--drawer-pad-x) var(--drawer-pad-x); }
    .menu-content::-webkit-scrollbar { width: 4px; }
    .menu-content::-webkit-scrollbar-thumb { background: rgba(255,255,255,0.2); border-radius: 2px; }
    .menu-panel { display: none; animation: fade 0.2s; }
    .menu-panel.active { display: block; }
    @keyframes fade { from { opacity: 0; } to { opacity: 1; } }

    .menu-row { display: flex; justify-content: space-between; align-items: center; padding: calc(var(--drawer-pad-y) * 0.9) 0; border-bottom: 1px solid rgba(255,255,255,0.05); font-size: var(--panel-font); }
    .menu-label { color: var(--muted); }
    .menu-val { font-weight: 500; }
    .menu-val-wide { font-size: clamp(9px, calc(var(--ui-short) * 0.023), 10px); max-width: min(52%, clamp(150px, calc(var(--ui-width) * 0.18), 180px)); text-align: right; }
    .menu-actions { display: flex; gap: calc(var(--cluster-gap) * 0.66); margin-top: calc(var(--drawer-pad-x) * 0.9); }
    .menu-actions.compact { margin-top: calc(var(--drawer-pad-y) * 0.66); margin-bottom: calc(var(--drawer-pad-y) * 0.9); }
    .menu-search { width: 100%; background: rgba(0,0,0,0.3); border: 1px solid rgba(255,255,255,0.2); color: #fff; padding: var(--input-pad); border-radius: 6px; margin-bottom: calc(var(--drawer-pad-y) * 0.9); font-size: var(--panel-font); }
    .wave-legend { display: flex; gap: calc(var(--cluster-gap) * 0.66); flex-wrap: wrap; margin-top: calc(var(--drawer-pad-y) * 0.66); font-size: var(--label-font); }
    
    .btn { background: rgba(255,255,255,0.1); border: 1px solid rgba(255,255,255,0.1); color: #fff; padding: var(--button-pad-y) var(--button-pad-x); border-radius: 6px; font-size: var(--button-font); cursor: pointer; transition: background 0.2s; }
    .btn:active { background: rgba(255,255,255,0.2); }
    .btn.primary { background: rgba(48,228,197,0.15); border-color: rgba(48,228,197,0.3); color: var(--green); }
    
    /* Config items inside menu */
    .param-row { display: flex; align-items: center; padding: calc(var(--drawer-pad-y) * 0.55) 0; gap: calc(var(--cluster-gap) * 0.66); border-bottom: 1px solid rgba(255,255,255,0.05); }
    .param-name { flex: 1; font-size: var(--panel-font); }
    .param-row input { background: rgba(0,0,0,0.3); border: 1px solid rgba(255,255,255,0.2); color: #fff; padding: var(--input-pad); border-radius: 4px; width: var(--input-width); text-align: center; }

    /* Hidden elements strictly needed by JS logic */
    .hidden { display: none !important; }

    .toast { position: fixed; top: var(--toast-top); left: 50%; transform: translateX(-50%) translateY(-20px); background: rgba(20,25,30,0.8); backdrop-filter: blur(10px); -webkit-backdrop-filter: blur(10px); padding: calc(var(--button-pad-y) + 2px) var(--telemetry-pad-x); border-radius: var(--telemetry-radius); opacity: 0; visibility: hidden; transition: all 0.3s; z-index: 200; font-size: var(--button-font); border: 1px solid rgba(255,255,255,0.1); display: flex; align-items: center; color: #fff; }
    .toast.show { opacity: 1; visibility: visible; transform: translateX(-50%) translateY(0); }
  
    /* Force landscape via media query if portrait */
    @media screen and (orientation: portrait) {
      html, body {
        width: 100vw;
        height: 100vh;
      }
      body {
        inset: auto;
        width: var(--vh);
        height: var(--vw);
        top: 50%;
        left: 50%;
        right: auto;
        bottom: auto;
        transform: translate(-50%, -50%) rotate(90deg);
        transform-origin: center center;
      }
    }
</style>

</head>
<body>
  
  <div class="video-stage">
    <div class="video-feed" id="videoFeed">
      <div id="videoPlaceholder">
        <div>
          <strong class="placeholder-title">相机未连接</strong>
          <span id="videoNote" class="placeholder-note">正在等待图传画面...</span>
        </div>
      </div>
    </div>
  </div>

  <!-- Reticle -->
  <div class="center-reticle">
    <div class="af-box"></div>
  </div>

  <!-- Fake Scales -->
  <div class="HUD-scale scale-left">
    <div class="hud-line" data-val="10"></div>
    <div class="hud-line" data-val="0"></div>
    <div class="hud-line" data-val="-10"></div>
  </div>
  <div class="HUD-scale scale-right">
    <div class="hud-line" data-val="10"></div>
    <div class="hud-line" data-val="0"></div>
    <div class="hud-line" data-val="-10"></div>
  </div>

  <!-- Top Bar -->
  <div class="top-bar">
    <div class="top-left">
      <img class="top-logo" src="data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='24' height='24' viewBox='0 0 24 24' fill='white'><path d='M2.38 10L12 18.67 21.62 10 12 1.33 2.38 10zm19.24 3.05l-1.63-1.47-7.99 7.21-7.99-7.21-1.63 1.47 9.62 8.68 9.62-8.68z'/></svg>">
      <button class="status-badge tap-badge arm-button" id="armButton" type="button">
        <span id="armValue">SAFE</span>
      </button>
      <div class="mode-anchor">
        <div class="status-badge tap-badge" id="modeBadge">
          <span class="icon-label">模式</span>
          <span id="modeValue">手动</span>
        </div>
        <div class="mode-menu" id="modeMenu">
          <button class="mode-option active" data-flightmode="0">
            <span>手动</span>
            <small>RATE</small>
          </button>
          <button class="mode-option" data-flightmode="1">
            <span>自稳</span>
            <small>ANGLE</small>
          </button>
          <button class="mode-option" data-flightmode="2">
            <span>ALT</span>
            <small>ALT HOLD</small>
          </button>
        </div>
      </div>
    </div>
    
    <div class="top-right">
      <div class="status-badge">
        <span class="icon-label">链路</span>
        <span id="wifiState" style="font-weight:600;">--</span>
        <div id="wifiDot" class="status-dot"></div>
      </div>
      <div class="status-badge">
        <span class="icon-label">控制</span>
        <span id="rcValue" style="color:var(--green); font-weight:600;">--</span>
      </div>
      <div class="status-badge">
        <span class="icon-label" id="cameraState">图传</span>
      </div>
      <div class="menu-btn" id="openMenuBtn">
        <div><span></span><span></span><span></span></div>
      </div>
    </div>
  </div>

  <!-- Radar -->
  <div class="radar" id="radar">
    <div class="radar-inner">
      <div class="attitude-north">N</div>
      <div class="attitude-horizon" id="attitudePitch">
        <div class="attitude-horizon-roll" id="attitudeRoll">
          <div class="attitude-sky"></div>
          <div class="attitude-ground"></div>
          <div class="attitude-line"></div>
          <div class="attitude-ladder attitude-ladder-up"></div>
          <div class="attitude-ladder attitude-ladder-down"></div>
        </div>
      </div>
      <div class="attitude-bank"></div>
      <div class="attitude-heading" id="attitudeHeading">
        <div class="attitude-heading-arrow"></div>
      </div>
      <div class="attitude-marker">
        <span class="attitude-wing left"></span>
        <span class="attitude-center"></span>
        <span class="attitude-wing right"></span>
      </div>
    </div>
  </div>

  <!-- Bottom Telemetry -->
  <div class="bottom-telemetry">
    <div class="telemetry-item">
      <span class="t-label">高度</span>
      <span class="t-val" id="altValue">--</span>
      <span class="t-label">米</span>
    </div>
    <div class="telemetry-divider"></div>
    <div class="telemetry-item">
      <span class="t-label">加计</span>
      <span class="t-val" id="accelValue">--</span>
    </div>
    <div class="telemetry-item">
      <span class="t-label">角速</span>
      <span class="t-val" id="gyroValue">--</span>
    </div>
    <div class="telemetry-divider"></div>
    <div class="telemetry-item">
      <span class="t-label">频率</span>
      <span class="t-val" id="rateValue">--</span>
    </div>
    <div class="telemetry-divider"></div>
    <div class="telemetry-item">
      <span class="t-label">油门</span>
      <span class="t-val" id="throttleValue">--</span>
    </div>
  </div>

  <!-- Right Tools -->
  <div class="right-tools">
    <div class="tool-btn" id="quickSaveBtn" title="保存参数">
      <svg class="tool-icon" viewBox="0 0 24 24" fill="white"><path d="M17 3H5a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14c1.1 0 2-.9 2-2V7l-4-4zm-5 16c-1.66 0-3-1.34-3-3s1.34-3 3-3 3 1.34 3 3-1.34 3-3 3zm3-10H5V5h10v4z"/></svg>
    </div>
    <div class="mode-switch" id="modeSwitch">
      <button class="mode-chip active" id="modeRateBtn" data-flightmode="0">RATE</button>
      <button class="mode-chip" id="modeAngleBtn" data-flightmode="1">ANGLE</button>
      <button class="mode-chip" id="modeAltBtn" data-flightmode="2">ALT</button>
    </div>
    <div class="control-pill">
      <span class="control-pill-label">控制源</span>
      <span class="control-pill-value" id="controlSourceValue">待机</span>
    </div>
  </div>

  <div class="joystick left" id="leftStick">
    <div class="joystick-title">升降 / 偏航</div>
    <div class="joystick-base joystick-cross"></div>
    <div class="joystick-ring"></div>
    <div class="joystick-knob" id="leftStickKnob"></div>
    <div class="joystick-readout" id="leftStickValue">升 0.00  偏 0.00</div>
  </div>

  <div class="joystick right" id="rightStick">
    <div class="joystick-title">横滚 / 俯仰</div>
    <div class="joystick-base joystick-cross"></div>
    <div class="joystick-ring"></div>
    <div class="joystick-knob" id="rightStickKnob"></div>
    <div class="joystick-readout" id="rightStickValue">俯 0.00  滚 0.00</div>
  </div>

  <!-- Menu Overlay -->
  <div class="menu-backdrop" id="menuBackdrop"></div>
  <aside class="menu-drawer" id="menuDrawer">
    <div class="menu-header">
      <h2>高级设置</h2>
      <button class="close-btn" id="closeMenuBtn">&times;</button>
    </div>
    
    <nav class="menu-tabs">
      <button class="menu-tab active" data-panel="system">系统</button>
      <button class="menu-tab" data-panel="status">传感器</button>
      <button class="menu-tab" data-panel="params">飞行参数</button>
      <button class="menu-tab" data-panel="wave">信号波形</button>
    </nav>

    <div class="menu-content">
      <!-- System Panel -->
      <div class="menu-panel active" data-panel="system">
        <div class="menu-row"><span class="menu-label">IP 地址</span><span class="menu-val" id="menuIpValue">--</span></div>
        <div class="menu-row"><span class="menu-label">网络名称</span><span class="menu-val" id="ssidValue">--</span></div>
        <div class="menu-row"><span class="menu-label">视频传输</span><span class="menu-val" id="menuVideoTransport">--</span></div>
        <div class="menu-row"><span class="menu-label">视频源</span><span class="menu-val menu-val-wide" id="menuVideoSource">--</span></div>
        <div class="menu-actions">
          <button class="btn primary" id="openVideoHookBtn" style="flex:1">自动连接视频</button>
          <button class="btn" id="clearVideoBtn" style="flex:1">断开视频</button>
        </div>
      </div>

      <!-- Sensor Panel -->
      <div class="menu-panel" data-panel="status">
        <div class="menu-row"><span class="menu-label">惯性传感器 (IMU)</span><span class="menu-val" id="menuImuValue">--</span></div>
        <div class="menu-row"><span class="menu-label">气压计 (BARO)</span><span class="menu-val" id="menuBarValue">--</span></div>
        <div class="menu-row"><span class="menu-label">磁力计 (MAG)</span><span class="menu-val" id="menuMagValue">--</span></div>
        <div class="menu-row"><span class="menu-label">横滚角 (Roll)</span><span class="menu-val" id="rollValue">--</span></div>
        <div class="menu-row"><span class="menu-label">俯仰角 (Pitch)</span><span class="menu-val" id="pitchValue">--</span></div>
        <div class="menu-row"><span class="menu-label">偏航角 (Yaw)</span><span class="menu-val" id="yawValue">--</span></div>
        <div class="menu-row"><span class="menu-label">电机1输出</span><span class="menu-val" id="m1Value">--</span></div>
        <div class="menu-row"><span class="menu-label">电机2输出</span><span class="menu-val" id="m2Value">--</span></div>
      </div>

      <!-- Params Panel -->
      <div class="menu-panel" data-panel="params">
        <div class="menu-actions compact">
          <button class="btn" id="reloadParamsBtn" style="flex:1">刷新参数</button>
          <button class="btn primary" id="saveParamsBtn" style="flex:1">保存到闪存</button>
        </div>
        <input type="text" id="paramSearch" class="menu-search" placeholder="搜索参数...">
        <div id="paramList"></div>
      </div>

      <!-- Wave Panel -->
      <div class="menu-panel" data-panel="wave">
        <div class="menu-actions compact">
          <button class="btn" id="fitWaveBtn" style="flex:1">自适应视图</button>
          <button class="btn" id="pauseWaveBtn" style="flex:1">暂停显示</button>
        </div>
        <canvas id="waveCanvas" style="width:100%; height:var(--wave-height); background:rgba(0,0,0,0.3); border:1px solid rgba(255,255,255,0.1); border-radius:8px;"></canvas>
        <div id="waveLegend" class="wave-legend"></div>
      </div>
    </div>
  </aside>
  
  <div class="toast" id="toast"></div>

  <!-- Hidden legacy elements required by existing script to avoid null errors -->
  <div class="hidden">
    <span id="domainValue"></span><span id="footerMessage"></span><span id="thrValue"></span><span id="footerLink"></span><span id="footerVideo"></span><span id="urlValue"></span>
    <span id="imuName"></span><span id="barName"></span><span id="magName"></span><span id="videoTransport"></span><span id="videoSource"></span><span id="footerArm"></span>
    <span id="footerSave"></span><span id="ipValue"></span><span id="menuGyroValue"></span><span id="hotspotNote"></span><span id="yawCompassValue"></span>
    <span id="magValue"></span><span id="m3Value"></span><span id="m4Value"></span><span id="menuHostValue"></span><span id="menuPreferredUrl"></span>
    <span id="menuCameraState"></span><button id="refreshWaveBtn"></button><span id="waveState"></span>
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
        modeMenuOpen: false,
        activePanel: 'system',
        overviewBusy: false,
        waveBusy: false,
        overviewTimer: 0,
        waveTimer: 0,
        overviewErrorShown: false,
        lastTelemetryRenderMs: 0,
        control: {
          linkEnabled: false,
          txBusy: false,
          txTimer: 0,
          txQueued: false,
          dirty: false,
          intentSeq: 0,
          clientId: Math.floor(Math.random() * 2147483646) + 1,
          priorityUntilMs: 0,
          lastSentMs: 0,
          lastIntentMs: 0,
          armed: false,
          flightmode: 0,
          vspeed: 0,
          roll: 0,
          pitch: 0,
          yaw: 0,
          leftPointerId: null,
          rightPointerId: null,
          armPointerId: null,
          armHoldTimer: 0,
          armHoldLatched: false,
        },
      };

      const kOverviewPollActiveMs = 260;
      const kOverviewPollHiddenMs = 700;
      const kOverviewPollControlActiveMs = 500;
      const kOverviewPollControlHiddenMs = 1000;
      const kBottomTelemetryActiveMs = 320;
      const kBottomTelemetryHiddenMs = 900;
      const kControlMinTxMs = 100;
      const kControlHeartbeatMs = 120;
      const kControlOverviewQuietMs = 90;
      const kControlPriorityPauseMs = 500;
      const kArmHoldMs = 900;
      const kWavePollActiveMs = 120;
      const kWavePollHiddenMs = 360;
      const kUiVersion = '0528-footer-throttle-v1';

      const els = {};
      const q = (id) => document.getElementById(id);

      async function fetchJson(url, options) {
        const request = Object.assign({ cache: 'no-store' }, options || {});
        const response = await fetch(url, request);
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

      function fmtTripletCompact(a, b, c, digits = 2) {
        return `${fmt(a, digits)},${fmt(b, digits)},${fmt(c, digits)}`;
      }

      function clamp(value, min, max) {
        return Math.max(min, Math.min(max, value));
      }

      function isPortraitRotatedUi() {
        return !!(window.matchMedia && window.matchMedia('(orientation: portrait)').matches);
      }

      function joystickControlVector(nx, ny) {
        const sx = clamp(nx, -1, 1);
        const sy = clamp(ny, -1, 1);
        if (isPortraitRotatedUi()) {
          return { x: sy, y: -sx };
        }
        return { x: sx, y: sy };
      }

      function joystickVisualVector(nx, ny) {
        const sx = clamp(nx, -1, 1);
        const sy = clamp(ny, -1, 1);
        return { x: sx, y: sy };
      }

      function flightModeLabel(mode) {
        if (Number(mode) === 2) return 'ALT';
        return Number(mode) === 1 ? '自稳' : '手动';
      }

      function setModeMenuOpen(open) {
        state.modeMenuOpen = !!open;
        if (els.modeBadge) els.modeBadge.classList.toggle('open', state.modeMenuOpen);
        if (els.modeMenu) els.modeMenu.classList.toggle('open', state.modeMenuOpen);
      }

      function markControlLinkActive(markDirty = true) {
        state.control.linkEnabled = true;
        if (markDirty) {
          state.control.intentSeq += 1;
          state.control.dirty = true;
        }
      }

      function currentOverviewPollMs() {
        if (state.control.linkEnabled) {
          return document.hidden ? kOverviewPollControlHiddenMs : kOverviewPollControlActiveMs;
        }
        return document.hidden ? kOverviewPollHiddenMs : kOverviewPollActiveMs;
      }

      function controlQuietRemainingMs() {
        if (!state.control.linkEnabled) return 0;
        if (state.control.txBusy) return kControlOverviewQuietMs;
        const lastControlMs = Math.max(state.control.lastIntentMs || 0, state.control.lastSentMs || 0);
        if (!lastControlMs) return 0;
        const elapsed = performance.now() - lastControlMs;
        return elapsed < kControlOverviewQuietMs ? kControlOverviewQuietMs - elapsed : 0;
      }

      function currentBottomTelemetryMs() {
        return document.hidden ? kBottomTelemetryHiddenMs : kBottomTelemetryActiveMs;
      }

      function currentWavePollMs() {
        return document.hidden ? kWavePollHiddenMs : kWavePollActiveMs;
      }

      function scheduleOverview(delay = currentOverviewPollMs()) {
        window.clearTimeout(state.overviewTimer);
        state.overviewTimer = window.setTimeout(refreshOverview, Math.max(20, delay, controlQuietRemainingMs()));
      }

      function scheduleWave(delay = currentWavePollMs()) {
        window.clearTimeout(state.waveTimer);
        state.waveTimer = window.setTimeout(refreshWave, Math.max(30, delay));
      }

      function scheduleControl(delay = kControlHeartbeatMs) {
        window.clearTimeout(state.control.txTimer);
        if (!state.control.linkEnabled) return;
        const priorityRemaining = Math.max(0, (state.control.priorityUntilMs || 0) - performance.now());
        state.control.txTimer = window.setTimeout(() => pushControl(false), Math.max(30, delay, priorityRemaining));
      }

      function toast(message, timeout = 1800) {
        const el = els.toast;
        el.textContent = message;
        el.classList.add('show');
        window.clearTimeout(toast._timer);
        toast._timer = window.setTimeout(() => el.classList.remove('show'), timeout);
      }

      function syncViewportMetrics() {
        const vv = window.visualViewport;
        const width = Math.round(vv ? vv.width : window.innerWidth);
        const height = Math.round(vv ? vv.height : window.innerHeight);
        const uiWidth = Math.max(width, height);
        const uiHeight = Math.min(width, height);
        document.documentElement.style.setProperty('--vw', `${width}px`);
        document.documentElement.style.setProperty('--vh', `${height}px`);
        document.documentElement.style.setProperty('--ui-width', `${uiWidth}px`);
        document.documentElement.style.setProperty('--ui-height', `${uiHeight}px`);
        document.documentElement.style.setProperty('--ui-short', `${uiHeight}px`);
        document.documentElement.style.setProperty('--ui-long', `${uiWidth}px`);
        if (els.leftStickKnob) syncControlUi();
      }

      async function tryLockLandscapeRight() {
        const orientation = screen.orientation;
        if (!orientation || typeof orientation.lock !== 'function') return;
        try {
          await orientation.lock('landscape-primary');
        } catch (error) {
        }
      }

      async function tryEnterFullscreen() {
        const root = document.documentElement;
        if (!root || document.fullscreenElement || typeof root.requestFullscreen !== 'function') return;
        try {
          await root.requestFullscreen({ navigationUI: 'hide' });
        } catch (error) {
          try {
            await root.requestFullscreen();
          } catch (ignored) {
          }
        }
      }

      function buildControlPayload(seq = state.control.intentSeq) {
        const form = new URLSearchParams();
        form.set('vspeed', state.control.vspeed.toFixed(4));
        form.set('roll', state.control.roll.toFixed(4));
        form.set('pitch', state.control.pitch.toFixed(4));
        form.set('yaw', state.control.yaw.toFixed(4));
        form.set('armed', state.control.armed ? '1' : '0');
        form.set('flightmode', String(state.control.flightmode || 0));
        form.set('client_id', String(state.control.clientId));
        form.set('seq', String(seq));
        form.set('ui_version', kUiVersion);
        return form;
      }

      function zeroControlAxes() {
        state.control.vspeed = 0;
        state.control.roll = 0;
        state.control.pitch = 0;
        state.control.yaw = 0;
      }

      function sendDisarmBeacon() {
        state.control.intentSeq += 1;
        const payload = buildControlPayload(state.control.intentSeq);
        payload.set('vspeed', '0.0000');
        payload.set('roll', '0.0000');
        payload.set('pitch', '0.0000');
        payload.set('yaw', '0.0000');
        payload.set('armed', '0');

        if (navigator.sendBeacon) {
          navigator.sendBeacon('/api/control', payload);
          return;
        }

        fetch('/api/control', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8' },
          body: payload.toString(),
          keepalive: true,
          cache: 'no-store',
        }).catch(() => {});
      }

      function setModeButtonState(mode) {
        const selectedMode = clamp(Number(mode) || 0, 0, 2);
        if (els.modeRateBtn) els.modeRateBtn.classList.toggle('active', selectedMode === 0);
        if (els.modeAngleBtn) els.modeAngleBtn.classList.toggle('active', selectedMode === 1);
        if (els.modeAltBtn) els.modeAltBtn.classList.toggle('active', selectedMode === 2);
        document.querySelectorAll('.mode-option').forEach((option) => {
          option.classList.toggle('active', Number(option.dataset.flightmode || 0) === selectedMode);
        });
        setText('modeValue', flightModeLabel(selectedMode));
      }

      function positionJoystick(surface, knob, nx, ny) {
        if (!surface || !knob) return;
        const radius = Math.max(0, surface.clientWidth * 0.5 - knob.clientWidth * 0.5 - 8);
        const translated = joystickVisualVector(nx, ny);
        knob.style.transform = `translate(${(translated.x * radius).toFixed(1)}px, ${(translated.y * radius).toFixed(1)}px)`;
      }

      function syncControlUi() {
        const leftY = -state.control.vspeed;
        positionJoystick(els.leftStick, els.leftStickKnob, state.control.yaw, leftY);
        positionJoystick(els.rightStick, els.rightStickKnob, state.control.roll, -state.control.pitch);
        setText('leftStickValue', `\u5347 ${fmt(state.control.vspeed, 2)}  \u504f ${fmt(state.control.yaw, 2)}`);
        setText('rightStickValue', `\u4fef ${fmt(state.control.pitch, 2)}  \u6eda ${fmt(state.control.roll, 2)}`);
        if (els.armButton) els.armButton.classList.toggle('armed', !!state.control.armed);
        setText('armValue', state.control.armed ? '\u5df2\u89e3\u9501' : '\u672a\u89e3\u9501');
        setModeButtonState(state.control.flightmode || 0);
      }

      function requestControlFlush(immediate = false) {
        if (!state.control.linkEnabled) return;
        state.control.lastIntentMs = performance.now();
        state.control.txQueued = true;

        if (state.control.txBusy) return;

        const now = performance.now();
        const wait = immediate ? 0 : Math.max(0, kControlMinTxMs - (now - state.control.lastSentMs));
        window.clearTimeout(state.control.txTimer);
        state.control.txTimer = window.setTimeout(() => pushControl(false), wait);
      }

      async function pushControl(force = false) {
        if (!state.control.linkEnabled || document.hidden) return;
        const priorityRemaining = !state.control.dirty
          ? Math.max(0, (state.control.priorityUntilMs || 0) - performance.now())
          : 0;
        if (priorityRemaining > 0) {
          scheduleControl(priorityRemaining);
          return;
        }
        if (state.control.txBusy) {
          state.control.txQueued = true;
          return;
        }

        state.control.txQueued = false;
        state.control.txBusy = true;
        const requestIntentSeq = state.control.intentSeq;
        state.control.lastSentMs = performance.now();
        state.control.lastIntentMs = state.control.lastSentMs;
        let ackedCurrent = false;
        try {
          const response = await fetch('/api/control', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8' },
            body: buildControlPayload(requestIntentSeq).toString(),
            cache: 'no-store',
          });

          const data = await response.json();

          if (data.ok && state.control.intentSeq === requestIntentSeq) {
            state.control.armed = !!data.armed;
            state.control.flightmode = Number(data.flightmode || 0);
            state.control.dirty = false;
            ackedCurrent = true;
          }
        } catch (error) {
          if (force) {
            toast(`控制发送失败: ${error.message}`, 2200);
          }
        } finally {
          state.control.txBusy = false;
          syncControlUi();
          if (state.control.txQueued || state.control.dirty) {
            requestControlFlush(false);
          } else if (!state.control.armed && ackedCurrent) {
            state.control.linkEnabled = false;
          } else {
            scheduleControl(kControlHeartbeatMs);
          }
        }
      }

      function setFlightmode(mode) {
        state.control.flightmode = clamp(Number(mode) || 0, 0, 2);
        markControlLinkActive();
        state.control.priorityUntilMs = performance.now() + kControlPriorityPauseMs;
        setModeMenuOpen(false);
        syncControlUi();
        requestControlFlush(true);
      }

      function setArmed(armed, message) {
        state.control.armed = !!armed;
        if (!state.control.armed) {
          zeroControlAxes();
        }
        markControlLinkActive();
        state.control.priorityUntilMs = performance.now() + kControlPriorityPauseMs;
        syncControlUi();
        if (!state.control.armed) {
          sendDisarmBeacon();
        }
        requestControlFlush(true);
        if (message) toast(message, 1400);
      }

      function installJoystick(surface, kind) {
        if (!surface) return;

        const pointerKey = kind === 'left' ? 'leftPointerId' : 'rightPointerId';

        function applyPointer(clientX, clientY) {
          const rect = surface.getBoundingClientRect();
          const cx = rect.left + rect.width / 2;
          const cy = rect.top + rect.height / 2;
          const radius = Math.max(1, rect.width / 2 - Math.min(rect.width, rect.height) * 0.18);

          let dx = (clientX - cx) / radius;
          let dy = (clientY - cy) / radius;
          const mag = Math.hypot(dx, dy);
          if (mag > 1) {
            dx /= mag;
            dy /= mag;
          }

          const input = joystickControlVector(dx, dy);

          markControlLinkActive();

          if (kind === 'left') {
            state.control.yaw = input.x;
            state.control.vspeed = clamp(-input.y, -1, 1);
          } else {
            state.control.roll = input.x;
            state.control.pitch = clamp(-input.y, -1, 1);
          }

          syncControlUi();
          requestControlFlush(false);
        }

        function releasePointer() {
          if (kind === 'left') {
            state.control.yaw = 0;
            state.control.vspeed = 0;
          } else {
            state.control.roll = 0;
            state.control.pitch = 0;
          }
          syncControlUi();
          requestControlFlush(true);
        }

        surface.addEventListener('pointerdown', (event) => {
          state.control[pointerKey] = event.pointerId;
          if (surface.setPointerCapture) {
            try { surface.setPointerCapture(event.pointerId); } catch (error) {}
          }
          applyPointer(event.clientX, event.clientY);
        });

        surface.addEventListener('pointermove', (event) => {
          if (state.control[pointerKey] !== event.pointerId) return;
          applyPointer(event.clientX, event.clientY);
        });

        ['pointerup', 'pointercancel', 'lostpointercapture'].forEach((type) => {
          surface.addEventListener(type, (event) => {
            if (state.control[pointerKey] !== event.pointerId) return;
            state.control[pointerKey] = null;
            releasePointer();
          });
        });
      }

      function installArmButton() {
        const button = els.armButton;
        if (!button) return;

        function clearHold() {
          window.clearTimeout(state.control.armHoldTimer);
          state.control.armHoldTimer = 0;
          button.classList.remove('holding');
        }

        button.addEventListener('pointerdown', (event) => {
          event.preventDefault();
          event.stopPropagation();
          state.control.armPointerId = event.pointerId;
          state.control.armHoldLatched = false;
          if (button.setPointerCapture) {
            try { button.setPointerCapture(event.pointerId); } catch (error) {}
          }

          if (state.control.armed) return;

          button.classList.add('holding');
          state.control.armHoldTimer = window.setTimeout(() => {
            if (state.control.armPointerId !== event.pointerId || state.control.armed) return;
            state.control.armHoldLatched = true;
            clearHold();
            setArmed(true, '\u5df2\u89e3\u9501');
          }, kArmHoldMs);
        });

        ['pointerup', 'pointercancel', 'lostpointercapture'].forEach((type) => {
          button.addEventListener(type, (event) => {
            if (state.control.armPointerId !== event.pointerId) return;
            state.control.armPointerId = null;
            clearHold();

            if (type !== 'pointerup') return;
            if (state.control.armHoldLatched) return;

            if (state.control.armed) {
              setArmed(false, '\u5df2\u4e0a\u9501');
            } else {
              toast('\u957f\u6309\u89e3\u9501', 1200);
            }
          });
        });
      }

      function openMenu(panel = state.activePanel) {
        setModeMenuOpen(false);
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
          if (event.key === 'Escape') {
            setModeMenuOpen(false);
            closeMenu();
          }
        });

        document.addEventListener('pointerdown', (event) => {
          if (!state.modeMenuOpen) return;
          const target = event.target;
          if ((els.modeBadge && els.modeBadge.contains(target)) || (els.modeMenu && els.modeMenu.contains(target))) return;
          setModeMenuOpen(false);
        });

        els.openVideoHookBtn.addEventListener('click', async () => {
          try {
            const info = await fetchJson('/api/video/config');
            toast(`图传接口已就绪: ${info.attach_api}`);
          } catch (error) {
            toast(`图传接口异常: ${error.message}`, 2600);
          }
        });

        els.clearVideoBtn.addEventListener('click', () => {
          window.MadflightUI.clearVideo();
          toast('已断开图传');
        });

        els.reloadParamsBtn.addEventListener('click', () => ensureParams(true));
        els.saveParamsBtn.addEventListener('click', saveParams);
        els.quickSaveBtn.addEventListener('click', saveParams);
        els.paramSearch.addEventListener('input', renderParams);
        els.modeRateBtn.addEventListener('click', () => setFlightmode(0));
        els.modeAngleBtn.addEventListener('click', () => setFlightmode(1));
        els.modeAltBtn.addEventListener('click', () => setFlightmode(2));
        document.querySelectorAll('.mode-option').forEach((option) => {
          option.addEventListener('click', () => setFlightmode(Number(option.dataset.flightmode || 0)));
        });
        els.modeBadge.addEventListener('click', (event) => {
          event.stopPropagation();
          setModeMenuOpen(!state.modeMenuOpen);
        });

        els.pauseWaveBtn.addEventListener('click', () => {
          state.wavePaused = !state.wavePaused;
          els.pauseWaveBtn.textContent = state.wavePaused ? '继续显示' : '暂停显示';
        });

        els.refreshWaveBtn.addEventListener('click', refreshWave);
        els.fitWaveBtn.addEventListener('click', () => {
          state.fitWave = !state.fitWave;
          els.fitWaveBtn.textContent = state.fitWave ? '自适应视图' : '固定视图';
          if (state.wave) drawWave(state.wave);
        });

        installJoystick(els.leftStick, 'left');
        installJoystick(els.rightStick, 'right');
      }

      function updateAttitude(rollDeg, pitchDeg, yawDeg) {
        const roll = Number(rollDeg || 0);
        const pitch = Number(pitchDeg || 0);
        const yaw = Number(yawDeg || 0);
        const pitchScale = Math.max(0, els.radar.clientHeight || 0) * 0.12;
        const pitchOffset = Math.max(-30, Math.min(30, pitch)) * pitchScale / 10;

        els.attitudePitch.style.transform = `translate(-50%, -50%) translateY(${pitchOffset}px)`;
        els.attitudeRoll.style.transform = `rotate(${-roll}deg)`;
        els.attitudeHeading.style.transform = `rotate(${yaw}deg)`;
        setText('yawCompassValue', `R ${fmt(roll, 1)} P ${fmt(pitch, 1)} Y ${fmt(yaw, 1)}`);
      }

      function renderBottomTelemetry(data, force = false) {
        const now = performance.now();
        if (!force && state.lastTelemetryRenderMs && (now - state.lastTelemetryRenderMs) < currentBottomTelemetryMs()) {
          return;
        }

        state.lastTelemetryRenderMs = now;
        setText('rateValue', `${fmt(data.imu.sample_rate_hz, 0)} Hz`);
        setText('gyroValue', fmtTripletCompact(
          data.ahr.gx ?? data.imu.gx,
          data.ahr.gy ?? data.imu.gy,
          data.ahr.gz ?? data.imu.gz,
          1
        ));
        setText('accelValue', fmtTripletCompact(
          data.ahr.ax ?? data.imu.ax,
          data.ahr.ay ?? data.imu.ay,
          data.ahr.az ?? data.imu.az,
          2
        ));
        const altValue = data.alt && data.alt.h_m !== undefined ? data.alt.h_m : data.bar.alt_m;
        setText('altValue', `${fmt(altValue, 2)} m`);
      }

      function renderOverview(data) {
        const wifiOk = !!data.wifi_connected;
        const preferredUrl = data.preferred_url || (data.ip ? `http://${data.ip}/` : 'http://madflight.local/');
        const videoLive = !!(data.video && data.video.stream_url);
        const inputSourceLabel = data.flight.input_source === 'udp' ? 'UDP' : (data.flight.input_source === 'ui' ? '手机' : '等待');

        els.wifiDot.classList.toggle('ok', wifiOk);
        setText('wifiState', wifiOk ? '已连' : '等待');
        setText('armValue', data.flight.armed ? '已解锁' : '未解锁');
        setText('rcValue', inputSourceLabel);
        setText('footerMessage', wifiOk ? `Open ${preferredUrl}` : 'Waiting for hotspot link');

        setText('thrValue', fmt(data.flight.vspeed, 2));
        setText('throttleValue', fmt(data.flight.throttle, 2));
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
        setText('footerArm', data.flight.armed ? 'ARMED' : 'DISARMED');
        setText('cameraState', videoLive ? '图传直播' : '图传预留');
        setText('footerSave', els.footerSave.textContent || 'IDLE');
        setText('waveState', state.wave && !state.wavePaused ? '更新中' : '就绪');

        setText('ssidValue', data.wifi_ssid || '--');
        setText('ipValue', data.ip || '0.0.0.0');
        setText('videoTransport', data.video.transport || 'reserved');
        setText('videoSource', data.video.stream_url || '未接入');

        setText('m1Value', fmt(data.outputs.m1, 2));
        setText('m2Value', fmt(data.outputs.m2, 2));
        setText('m3Value', fmt(data.outputs.m3, 2));
        setText('m4Value', fmt(data.outputs.m4, 2));

        setModeButtonState(data.flight.mode_index);
        setText('magValue', `${fmt(data.mag.mx_ut, 1)} / ${fmt(data.mag.my_ut, 1)} / ${fmt(data.mag.mz_ut, 1)}`);
        updateAttitude(data.ahr.roll_deg, data.ahr.pitch_deg, data.ahr.yaw_deg);
        if (data.flight.input_source === 'ui' && !state.control.dirty && !state.control.txBusy && controlQuietRemainingMs() <= 0) {
          state.control.linkEnabled = true;
          state.control.armed = !!data.flight.armed;
          state.control.flightmode = Number(data.flight.mode_index || 0);
        }
        setText('controlSourceValue', data.flight.input_source === 'udp' ? 'UDP控制' : (data.flight.input_source === 'ui' ? '手机控制' : '手机待命'));
        syncControlUi();

        setText('menuHostValue', data.hostname_mdns || data.hostname || 'madflight.local');
        setText('menuIpValue', data.ip || '0.0.0.0');
        setText('menuPreferredUrl', preferredUrl);
        setText('hotspotNote', data.hotspot_note || 'Use the aircraft IP first when the hotspot does not resolve .local.');
        setText('menuImuValue', data.imu.name || '--');
        setText('menuBarValue', data.bar.name || '--');
        setText('menuMagValue', data.mag.name || '--');
        setText('menuGyroValue', `${fmt(data.imu.gx, 1)} / ${fmt(data.imu.gy, 1)} / ${fmt(data.imu.gz, 1)}`);
        setText('menuCameraState', videoLive ? '直播中' : '预留');
        setText('menuVideoTransport', data.video.transport || 'reserved');
        setText('menuVideoSource', data.video.stream_url || '未接入');
        renderBottomTelemetry(data);

        if (!state.videoSource && data.video.stream_url) {
          window.MadflightUI.attachVideoSource(data.video.stream_url, data.video.transport || 'img');
        }
      }

      async function refreshOverview() {
        const quietMs = controlQuietRemainingMs();
        if (quietMs > 0) {
          scheduleOverview(quietMs);
          return;
        }
        if(state.overviewBusy) return;
        state.overviewBusy = true;
        const startedAt = performance.now();
        try {
          const data = await fetchJson(`/api/overview?t=${Date.now()}`);
          state.overview = data;
          renderOverview(data);
          state.overviewErrorShown = false;
        } catch (error) {
          setText('wifiState', '异常');
          if(!state.overviewErrorShown) {
            toast(`状态获取失败: ${error.message}`, 2200);
            state.overviewErrorShown = true;
          }
        } finally {
          state.overviewBusy = false;
          const elapsed = performance.now() - startedAt;
          scheduleOverview(currentOverviewPollMs() - elapsed);
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
          toast(`已载入 ${data.items.length} 个参数`);
        } catch (error) {
          toast(`参数加载失败: ${error.message}`, 2600);
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
            toast(`参数应用失败: ${name}`);
            return;
          }

          setText('footerSave', 'DIRTY');
          toast(`已应用参数: ${name}`);
        } catch (error) {
          toast(`参数写入异常: ${error.message}`, 2600);
        }
      }

      async function saveParams() {
        try {
          const data = await fetchJson('/api/param/save', { method: 'POST' });
          if (data.ok) {
            setText('footerSave', 'SAVED');
            toast('参数已保存到闪存');
          } else {
            toast('参数保存失败', 2200);
          }
        } catch (error) {
          toast(`保存异常: ${error.message}`, 2600);
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
        if (!state.menuOpen || state.activePanel !== 'wave' || state.wavePaused) {
          scheduleWave();
          return;
        }
        if(state.waveBusy) return;
        state.waveBusy = true;
        const startedAt = performance.now();

        try {
          const data = await fetchJson(`/api/waveform?t=${Date.now()}`);
          state.wave = data;
          drawWave(data);
          setText('waveState', '更新中');
        } catch (error) {
          setText('waveState', '异常');
        } finally {
          state.waveBusy = false;
          const elapsed = performance.now() - startedAt;
          scheduleWave(currentWavePollMs() - elapsed);
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
              setText('videoSource', '未接入');
              setText('menuVideoSource', '未接入');
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
            setText('cameraState', '图传直播');
            setText('menuCameraState', '直播中');
          },

          clearVideo() {
            state.videoSource = '';
            const existing = els.videoFeed.querySelector('.attached-video');
            if (existing) existing.remove();
            els.videoPlaceholder.style.display = '';
            setText('videoSource', '未接入');
            setText('menuVideoSource', '未接入');
            setText('videoTransport', 'reserved');
            setText('menuVideoTransport', 'reserved');
            setText('footerVideo', 'READY');
            setText('cameraState', '图传预留');
            setText('menuCameraState', '预留');
          },

          setVideoNote(note) {
            setText('videoNote', note);
          },
        };
      }

      function cacheElements() {
        [
          'videoFeed', 'videoPlaceholder', 'videoNote', 'toast',
          'armButton',
          'wifiDot', 'wifiState', 'armValue', 'rcValue', 'openMenuBtn', 'radar',
          'footerMessage', 'thrValue', 'rateValue', 'throttleValue', 'footerLink', 'footerVideo',
          'domainValue', 'urlValue', 'rollValue', 'pitchValue', 'yawValue',
          'imuName', 'barName', 'magName', 'gyroValue', 'accelValue',
          'altValue', 'footerArm', 'cameraState', 'footerSave',
          'waveState', 'yawCompassValue', 'attitudePitch', 'attitudeRoll', 'attitudeHeading', 'magValue',
          'ssidValue', 'ipValue', 'videoTransport', 'videoSource',
          'm1Value', 'm2Value', 'm3Value', 'm4Value', 'modeValue', 'modeBadge', 'modeMenu',
          'menuBackdrop', 'menuDrawer', 'closeMenuBtn',
          'menuHostValue', 'menuIpValue', 'menuPreferredUrl', 'hotspotNote',
          'menuImuValue', 'menuBarValue', 'menuMagValue', 'menuGyroValue',
          'menuCameraState', 'menuVideoTransport', 'menuVideoSource',
          'openVideoHookBtn', 'clearVideoBtn',
          'paramSearch', 'paramList', 'reloadParamsBtn', 'saveParamsBtn', 'quickSaveBtn',
          'modeRateBtn', 'modeAngleBtn', 'modeAltBtn', 'controlSourceValue',
          'leftStick', 'leftStickKnob', 'leftStickValue', 'rightStick', 'rightStickKnob', 'rightStickValue',
          'pauseWaveBtn', 'refreshWaveBtn', 'fitWaveBtn', 'waveCanvas', 'waveLegend'
        ].forEach((id) => {
          els[id] = q(id);
        });
      }

      async function boot() {
        syncViewportMetrics();
        cacheElements();
        syncControlUi();
        bindButtons();
        installArmButton();
        installVideoHooks();
        tryLockLandscapeRight();
        await refreshOverview();
        toast(`界面 ${kUiVersion}`, 1200);
        scheduleWave();
        window.addEventListener('resize', syncViewportMetrics);
        window.addEventListener('orientationchange', syncViewportMetrics);
        document.addEventListener('visibilitychange', () => {
          if (document.hidden && state.control.linkEnabled) {
            state.control.armed = false;
            zeroControlAxes();
            syncControlUi();
            sendDisarmBeacon();
            state.control.linkEnabled = false;
            window.clearTimeout(state.control.txTimer);
          }
          scheduleOverview(document.hidden ? kOverviewPollHiddenMs : 20);
          scheduleWave(document.hidden ? kWavePollHiddenMs : 30);
        });
        window.addEventListener('pagehide', () => {
          if (!state.control.linkEnabled) return;
          state.control.armed = false;
          zeroControlAxes();
          sendDisarmBeacon();
        });
        window.addEventListener('pointerdown', () => {
          tryLockLandscapeRight();
          tryEnterFullscreen();
        }, { once: true, passive: true });
        if (window.visualViewport) {
          window.visualViewport.addEventListener('resize', syncViewportMetrics);
        }
      }

      window.addEventListener('DOMContentLoaded', boot);
    })();
  </script>
</body>
</html>
)USRUI";
