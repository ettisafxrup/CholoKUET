const fs = require("fs")
const path = require("path")
const root = path.join(__dirname, "..")
const GOLD = "#d9b100"
const INK = "#101114"
const PAPER = "#ffffff"
const MUTED = "#6b6d73"
const boundary = [
  [89.4958282, 22.9002338],
  [89.4970777, 22.9001738],
  [89.4972542, 22.8990155],
  [89.4983135, 22.899048],
  [89.499988, 22.8994079],
  [89.499988, 22.8974806],
  [89.5006102, 22.897493],
  [89.505481, 22.8976976],
  [89.506719, 22.8977425],
  [89.5065553, 22.8979068],
  [89.5062053, 22.8981094],
  [89.5054801, 22.8994513],
  [89.5023805, 22.9045742],
  [89.4994303, 22.9030198],
  [89.4982123, 22.9024679],
  [89.4982266, 22.902135],
  [89.4964869, 22.9020799],
  [89.4961593, 22.9021638],
  [89.4958313, 22.9007778],
]

function readLines(file) {
  return fs
    .readFileSync(path.join(root, file), "utf8")
    .split(/\r?\n/)
    .map((l) => l.trim())
    .filter((l) => l && !l.startsWith("#"))
}

const places = readLines("data/locations.txt").map((line) => {
  const [id, name, category, lat, lon] = line.split("|")
  return { id: +id, name, category, lat: +lat, lon: +lon }
})
const byId = new Map(places.map((p) => [p.id, p]))
const paths = readLines("data/paths.txt")
  .map((l) => l.split("|").map(Number))
  .filter(([a, b]) => byId.has(a) && byId.has(b))

const SCALE = 1.25
const lon0 = 89.4952,
  lat0 = 22.905
const mX = 102550,
  mY = 111320
const MAP_X = 40,
  MAP_Y = 150
const px = (lon) => MAP_X + (lon - lon0) * mX * SCALE
const py = (lat) => MAP_Y + (lat0 - lat) * mY * SCALE
const mapWidth = (89.5072 - lon0) * mX * SCALE
const mapHeight = (lat0 - 22.897) * mY * SCALE
const LEGEND_X = MAP_X + mapWidth + 40
const WIDTH = LEGEND_X + 440
const HEIGHT = MAP_Y + mapHeight + 70
const styles = {
  Academic: { fill: GOLD, stroke: GOLD, text: INK },
  Administrative: { fill: GOLD, stroke: GOLD, text: INK },
  Residential: { fill: INK, stroke: INK, text: GOLD },
  Facility: { fill: PAPER, stroke: INK, text: INK },
  Food: { fill: PAPER, stroke: INK, text: INK },
  Medical: { fill: PAPER, stroke: INK, text: INK },
  Religious: { fill: PAPER, stroke: INK, text: INK },
  Sports: { fill: PAPER, stroke: GOLD, text: INK },
  Landmark: { fill: PAPER, stroke: GOLD, text: INK },
  Transportation: { fill: INK, stroke: GOLD, text: PAPER },
}
const styleOf = (c) => styles[c] || { fill: PAPER, stroke: MUTED, text: INK }
const R = 10
const markers = places.map((p) => ({
  ...p,
  x: px(p.lon),
  y: py(p.lat),
  tx: px(p.lon),
  ty: py(p.lat),
}))
for (let round = 0; round < 200; round++) {
  let moved = false
  for (let i = 0; i < markers.length; i++) {
    for (let j = i + 1; j < markers.length; j++) {
      const a = markers[i],
        b = markers[j]
      let dx = b.x - a.x,
        dy = b.y - a.y
      let d = Math.hypot(dx, dy)
      const need = 2 * R + 3
      if (d < need) {
        if (d < 0.01) {
          dx = 1
          dy = 0
          d = 1
        }
        const push = (need - d) / 2
        a.x -= (dx / d) * push
        a.y -= (dy / d) * push
        b.x += (dx / d) * push
        b.y += (dy / d) * push
        moved = true
      }
    }
  }
  if (!moved) break
}

const esc = (s) =>
  s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;")
const out = []
out.push(
  `<svg xmlns="http://www.w3.org/2000/svg" width="${WIDTH.toFixed(0)}" height="${HEIGHT.toFixed(0)}" viewBox="0 0 ${WIDTH.toFixed(0)} ${HEIGHT.toFixed(0)}" font-family="Segoe UI, Helvetica, Arial, sans-serif">`,
)
out.push(`<rect width="100%" height="100%" fill="${PAPER}"/>`)
out.push(
  `<rect x="0" y="0" width="${WIDTH.toFixed(0)}" height="110" fill="${INK}"/>`,
)
out.push(
  `<text x="40" y="58" font-size="36" font-weight="700" fill="${GOLD}">KUET Campus Map</text>`,
)
out.push(
  `<text x="40" y="88" font-size="17" fill="${PAPER}" opacity="0.85">CholoKUET · Cholo Explore Kore Ashi · ${places.length} places, ${paths.length} walkable paths</text>`,
)
out.push(
  `<rect x="0" y="110" width="${WIDTH.toFixed(0)}" height="4" fill="${GOLD}"/>`,
)

const outline = boundary
  .map(([lon, lat]) => `${px(lon).toFixed(1)},${py(lat).toFixed(1)}`)
  .join(" ")
out.push(
  `<polygon points="${outline}" fill="#fbf6dc" stroke="${GOLD}" stroke-width="3" stroke-linejoin="round"/>`,
)
out.push(`<g stroke-linecap="round">`)
for (const [a, b] of paths) {
  const p = byId.get(a),
    q = byId.get(b)
  out.push(
    `<line x1="${px(p.lon).toFixed(1)}" y1="${py(p.lat).toFixed(1)}" x2="${px(q.lon).toFixed(1)}" y2="${py(q.lat).toFixed(1)}" stroke="${INK}" stroke-opacity="0.28" stroke-width="4"/>`,
  )
}
out.push(`</g>`)
out.push(
  `<text x="${px(89.501).toFixed(0)}" y="${(py(22.8975) + 30).toFixed(0)}" font-size="14" letter-spacing="6" fill="${MUTED}">TELIGATI ROAD</text>`,
)
out.push(
  `<text transform="translate(${(px(89.5058) + 34).toFixed(0)},${py(22.9012).toFixed(0)}) rotate(-58)" font-size="14" letter-spacing="4" fill="${MUTED}">KHULNA–JESSORE HIGHWAY SIDE</text>`,
)

for (const m of markers) {
  const s = styleOf(m.category)
  if (Math.hypot(m.x - m.tx, m.y - m.ty) > 3) {
    out.push(
      `<line x1="${m.tx.toFixed(1)}" y1="${m.ty.toFixed(1)}" x2="${m.x.toFixed(1)}" y2="${m.y.toFixed(1)}" stroke="${INK}" stroke-width="1"/>`,
    )
    out.push(
      `<circle cx="${m.tx.toFixed(1)}" cy="${m.ty.toFixed(1)}" r="2" fill="${INK}"/>`,
    )
  }
  out.push(
    `<circle cx="${m.x.toFixed(1)}" cy="${m.y.toFixed(1)}" r="${R}" fill="${s.fill}" stroke="${s.stroke}" stroke-width="2"/>`,
  )
  out.push(
    `<text x="${m.x.toFixed(1)}" y="${(m.y + 4).toFixed(1)}" font-size="${m.id > 9 ? 10 : 11}" font-weight="700" text-anchor="middle" fill="${s.text}">${m.id}</text>`,
  )
}
const nx = MAP_X + 40,
  ny = MAP_Y + 30
out.push(
  `<g transform="translate(${nx},${ny})"><polygon points="0,-22 9,10 0,4 -9,10" fill="${INK}"/><polygon points="0,-22 0,4 -9,10" fill="${GOLD}"/><text x="0" y="30" font-size="14" font-weight="700" text-anchor="middle" fill="${INK}">N</text></g>`,
)
const sx = MAP_X + 20,
  sy = MAP_Y + mapHeight + 30,
  bar = 100 * SCALE
out.push(
  `<rect x="${sx}" y="${sy}" width="${bar / 2}" height="8" fill="${INK}"/><rect x="${sx + bar / 2}" y="${sy}" width="${bar / 2}" height="8" fill="${GOLD}"/>`,
)
out.push(
  `<text x="${sx}" y="${sy - 6}" font-size="12" fill="${INK}">0</text><text x="${sx + bar}" y="${sy - 6}" font-size="12" text-anchor="middle" fill="${INK}">100 m</text>`,
)
out.push(
  `<text x="${MAP_X + mapWidth}" y="${sy + 8}" font-size="12" text-anchor="end" fill="${MUTED}">Map data © OpenStreetMap contributors (ODbL) · layout checked against the KUET master plan</text>`,
)
const order = readLines("data/categories.txt")
let ly = MAP_Y + 4
out.push(
  `<text x="${LEGEND_X}" y="${ly}" font-size="20" font-weight="700" fill="${INK}">Places</text>`,
)
ly += 14
for (const category of order) {
  const inCategory = places.filter(
    (p) => p.category.toLowerCase() === category.toLowerCase(),
  )
  if (!inCategory.length) continue
  const s = styleOf(category)
  ly += 22
  out.push(
    `<circle cx="${LEGEND_X + 7}" cy="${ly - 5}" r="7" fill="${s.fill}" stroke="${s.stroke}" stroke-width="2"/>`,
  )
  out.push(
    `<text x="${LEGEND_X + 22}" y="${ly}" font-size="14" font-weight="700" fill="${INK}">${esc(category.toUpperCase())}</text>`,
  )
  for (const p of inCategory) {
    ly += 17
    out.push(
      `<text x="${LEGEND_X + 22}" y="${ly}" font-size="13" fill="${INK}"><tspan font-weight="700" fill="${category === "Residential" ? INK : "#8a7000"}">${p.id}</tspan>  ${esc(p.name)}</text>`,
    )
  }
}
out.push(`</svg>`)

const target = path.join(root, "assets", "campus-map.svg")
fs.mkdirSync(path.dirname(target), { recursive: true })
fs.writeFileSync(target, out.join("\n"))
console.log(
  `Wrote ${target} (${places.length} places, ${paths.length} paths, legend ends at y=${ly})`,
)
