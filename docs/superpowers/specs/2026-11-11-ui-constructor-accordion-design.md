# UI Constructor — accordion declutter

**Date:** 2026-11-11  
**File:** `tools/ui-constructor.html`  
**Goal:** Reduce visual overload without removing any controls or Serial/export behavior.

## Context

Primary use is live USB tuning of arc and digit appearance. Pain: all parameters feel equally important in one long scroll. Primary groups: **Дуга**, **Шрифт**, **Превью**.

## Approach

Native `<details>`/`<summary>` accordions. Multiple sections may be open. Open state persisted in `sessionStorage`.

## Section defaults

| Section | Default | Notes |
|---------|---------|-------|
| USB | collapsed | Compact bar always visible: status + Connect / Disconnect / Demo |
| Дуга | open | All current arc controls |
| Шрифт | open | fontId + three sizes |
| Превью | open | RPM / speed / gear / HB |
| Обороты | collapsed | rpmMax / warn / red |
| Расположение | collapsed | Y + high beam |
| Boot | collapsed | Animation / timings |
| Экспорт | collapsed | gen / copy / reset + textarea |

## USB compact bar

When collapsed: status line + three buttons. When expanded: live-push checkbox + serial log (unchanged).

## Non-goals

No React/FSD rewrite. No removal of fields. No changes to `cfg()`, canvas draw, Web Serial protocol, or `exportDefaults()`.

## Done when

1. On load, Дуга + Шрифт + Превью are open and reachable without hunting.
2. Every previous control remains available.
3. Live USB and export behave as before.
4. Only `tools/ui-constructor.html` changes for the feature.
