<script>

    import L from 'leaflet';
    import MarkerPopup from './MarkerPopup.svelte';

    export let route;
    export let editable;

    let map;
    let error;

    function bindPopup(marker, createFn, dragFn) {
        let popupComponent;
        marker.bindPopup(() => {
            let container = L.DomUtil.create('div');
            popupComponent = createFn(container);
            return container;
        });

        marker.on('dragend', (e) => {
            dragFn(e.target.getLatLng());
        });


        marker.on('popupclose', () => {
            if (popupComponent) {
                let old = popupComponent;
                popupComponent = null;
                // Wait to destroy until after the fadeout completes.
                setTimeout(() => {
                    old.$destroy();
                }, 500);

            }
        });
    }

    function getInitialView(route) {
        if (route.track_points.length > 0) {
            let firstPoint = route.track_points[0];
            return [parseFloat(firstPoint.lat) || 0.0, parseFloat(firstPoint.lon) || 0.0];
        }
        return [55.8283, 37.5795];
    }

    function createLines() {
        let points = [];
        for (let point of route.track_points) {
            points.push([parseFloat(point.lat) || 0.0, parseFloat(point.lon) || 0.0]);
        }
        return L.polyline(points, {color: '#E4E', opacity: 0.5});
    }


    function createMap(container) {
        let initialView = getInitialView(route);
        let m = L.map(container, {preferCanvas: true}).setView(initialView, 10);
        m.attributionControl.setPrefix("");
        L.tileLayer(
            'https://{s}.basemaps.cartocdn.com/rastertiles/voyager/{z}/{x}/{y}{r}.png',
            {
                attribution: `&copy;<a href="https://www.openstreetmap.org/copyright" target="_blank">OpenStreetMap</a>,
	        &copy;<a href="https://carto.com/attributions" target="_blank">CARTO</a>`,
                subdomains: 'abcd',
                maxZoom: 14,
            }
        ).addTo(m);

        return m;
    }

    function mapInit() {
        map.eachLayer(function (layer) {
            if (layer instanceof L.LayerGroup) {
                map.removeLayer(layer);
            }
        });

        let markerLayers = L.layerGroup()

        let lines = createLines();
        for (let [i, wp] of route.waypoints.entries()) {
            let loc = [parseFloat(wp.lat) || 0.0, parseFloat(wp.lon) || 0.0];
            let marker = L.marker(loc, {
                draggable: editable,
            });
            bindPopup(marker, (m) => {
                let c = new MarkerPopup({
                    target: m,
                    props: {
                        title: wp.name,
                        desc: wp.desc,
                        comment: wp.comment,
                        editable: editable,
                    }
                });

                c.$on('change', (event) => {
                    const updatedRoute = event.detail;
                    route.waypoints[i].name = updatedRoute.title;
                    route.waypoints[i].desc = updatedRoute.desc;
                    route.waypoints[i].comment = updatedRoute.comment;
                });

                c.$on('delete', (event) => {
                    route.waypoints.splice(i, 1);
                    mapInit();
                });

                return c;
            }, (latlng) => {
                route.waypoints[i].lat = latlng.lat.toString();
                route.waypoints[i].lon = latlng.lng.toString();
            });

            markerLayers.addLayer(marker);
        }

        markerLayers.addTo(map);
        lines.addTo(map);
    }

    function mapAction(container) {
        map = createMap(container);

        mapInit();
    }

</script>
<link rel="stylesheet" href="https://unpkg.com/leaflet@1.6.0/dist/leaflet.css"
      integrity="sha512-xwE/Az9zrjBIphAcBb3F6JVqxf46+CDLwfLMHloNu6KEQCAWi6HcDUbeOfBIptF7tcCzusKFjFw2yuvEpDL9wQ=="
      crossorigin=""/>

<div class="map" style="height:400px;width:100%" use:mapAction></div>