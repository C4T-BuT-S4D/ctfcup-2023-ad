<script>
    import { onMount } from 'svelte';
    import world from '$lib/images/world.png';
    import {PUBLIC_BASE_URL} from '$env/static/public';
    export let routes = [];

    onMount(async () => {
        const response = await fetch(`${PUBLIC_BASE_URL}/route`, {credentials: 'include'});
        const data = await response.json();
        routes = data;
    });
</script>
<div>
    <img width="64" src={world} alt="world" />
    <p>Routes created by you:</p>
    <div class="content is-medium has-text-centered">
        {#each routes as route}
            <p><a href="/route/{route.id}">{route.title}</a></p>
        {/each}
    </div>

</div>
