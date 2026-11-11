# UI Constructor Accordion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Declutter `tools/ui-constructor.html` with accordions so primary live-tuning controls are open by default.

**Architecture:** Wrap existing sidebar sections in `<details class="sec">` with `<summary>` headers. Keep USB connect buttons outside the expandable body. Persist `open` state via `sessionStorage` key `enduro-ui-ctor-secs`. Leave all JS config/Serial/canvas logic unchanged except accordion helpers and USB status class sync.

**Tech Stack:** Single static HTML file, native `<details>`, CSS, vanilla JS.

## Global Constraints

- Do not remove or rename any control `id` used by `cfg()` / `IDS` / `CHK`.
- Do not change Serial command format or debounce (220 ms).
- Russian hardcoded labels stay (no i18n system in tools).
- No new dependencies.

---

### Task 1: Accordion markup + CSS + session persistence

**Files:**
- Modify: `tools/ui-constructor.html`

**Interfaces:**
- Consumes: existing element ids (`rpmMax`, `btnConnect`, …)
- Produces: `details.sec[data-sec]` with defaults; `restoreSections()` / `persistSections()`; compact USB bar markup

- [ ] **Step 1: Replace sidebar section headers with `<details>` structure**

Wrap sections in this pattern (ids and inputs unchanged):

```html
<details class="sec" data-sec="arc" open>
  <summary>Дуга</summary>
  <!-- existing labels -->
</details>
```

Order and `open`: usb (no open), arc open, font open, preview open, rpm collapsed, layout collapsed, boot collapsed, export collapsed.

USB structure:

```html
<details class="sec" data-sec="usb">
  <summary>Подключение USB</summary>
  <label class="chk">…livePush…</label>
  <div class="serial-log" id="serLog"></div>
</details>
<!-- Always-visible bar ABOVE or integrated: keep buttons + serStatus outside summary body but visually under title -->
```

Preferred layout for USB:

```html
<div class="usb-bar">
  <div class="row">buttons…</div>
  <div class="ok" id="serStatus">…</div>
</div>
<details class="sec" data-sec="usb">
  <summary>Лог и live-отправка</summary>
  …
</details>
```

Or keep summary "Подключение USB" and put buttons in a sticky `.usb-bar` before details. Spec: compact status + buttons always visible — implement as `.usb-bar` before the USB `<details>` whose summary is «Дополнительно USB» / «Лог Serial».

Use summary text: **«USB — лог»** for the collapsible part; keep h1 + short intro; place `.usb-bar` (buttons + status) immediately under intro.

- [ ] **Step 2: Add CSS for details/summary**

```css
.sec { border-top:1px solid var(--line); padding:4px 0 8px; }
.sec > summary {
  list-style:none; cursor:pointer; margin:12px 0 8px;
  font-size:12px; color:var(--muted); text-transform:uppercase; letter-spacing:.04em;
  display:flex; justify-content:space-between; align-items:center; user-select:none;
}
.sec > summary::-webkit-details-marker { display:none; }
.sec > summary::after { content:"▸"; color:var(--muted); }
.sec[open] > summary::after { content:"▾"; }
.usb-bar { display:grid; gap:8px; margin-bottom:8px; }
```

- [ ] **Step 3: Persist open state**

```js
const SEC_KEY = "enduro-ui-ctor-secs";
function persistSections() {
  const state = {};
  document.querySelectorAll("details.sec[data-sec]").forEach((el) => {
    state[el.dataset.sec] = el.open;
  });
  sessionStorage.setItem(SEC_KEY, JSON.stringify(state));
}
function restoreSections() {
  let state = null;
  try { state = JSON.parse(sessionStorage.getItem(SEC_KEY) || "null"); } catch (_) {}
  if (!state || typeof state !== "object") return;
  document.querySelectorAll("details.sec[data-sec]").forEach((el) => {
    if (Object.prototype.hasOwnProperty.call(state, el.dataset.sec)) {
      el.open = !!state[el.dataset.sec];
    }
  });
}
document.querySelectorAll("details.sec").forEach((el) => {
  el.addEventListener("toggle", persistSections);
});
restoreSections();
```

Call `restoreSections()` after DOM ready, before or after `bind()`.

- [ ] **Step 4: Manual verify in browser**

Run: `npx --yes serve D:/Project/Main/enduro-v2/tools -p 5173`  
Open `http://localhost:5173/ui-constructor.html`  
Expected: Дуга/Шрифт/Превью open; other sections closed; toggling survives F5; all sliders still update canvas; Connect buttons visible without expanding USB log.

- [ ] **Step 5: Commit (if git available)**

```bash
git add tools/ui-constructor.html docs/superpowers/specs/2026-11-11-ui-constructor-accordion-design.md docs/superpowers/plans/2026-11-11-ui-constructor-accordion.md
git commit -m "feat(tools): accordion sidebar for UI constructor"
```

If no `.git`, skip commit.
