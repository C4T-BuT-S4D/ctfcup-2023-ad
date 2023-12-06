<script>
    import {PUBLIC_BASE_URL} from "$env/static/public";
    import Map from '../../components/Map.svelte';
    import {invalidateAll} from "$app/navigation";

    /** @type {import('./$types').PageData} */
    export let data;

    let route = data.route;
    let error = data.error;

    let canEdit = route.user_id === data.user.id;

    let files;


    async function uploadGPX() {
        if (!files || files.length === 0) {
            return;
        }

        let formData = new FormData();
        formData.append('file', files[0]);

        const res = await fetch(`${PUBLIC_BASE_URL}/route/${route.id}/upload`, {
            method: 'POST',
            body: formData,
            credentials: 'include',
        });
        if (res.ok) {
            route = await res.json();
            files = null;
            await invalidateAll();
            return;
        }

        error = await res.text();
        try {
            error = JSON.parse(error).detail.error;
        } catch (e) {
            console.log(e);
        }
    }

    async function save() {
        const res = await fetch(`${PUBLIC_BASE_URL}/route/${route.id}/update`, {
            method: 'POST',
            body: JSON.stringify({
                title: route.title,
                description: route.description,
                track_points: route.track_points,
                waypoints: route.waypoints,
            }),
            credentials: 'include',
            headers: {
                'Content-Type': 'application/json'
            }
        });
        if (res.ok) {
            route = await res.json();
            files = null;
            await invalidateAll();
            return;
        }

        error = await res.text();
        try {
            error = JSON.parse(error).detail.error;
        } catch (e) {
            console.log(e);
        }
    }

</script>

{#if error}
    <div class="notification is-danger">
        {error}
    </div>
{/if}
<section class="hero">
    <div class="hero-body">
        <input class="input title is-static" type="text" bind:value={route.title} placeholder="Title" readonly={!canEdit}/>
        <div class="subtitle">
            <input class="input is-static" type="text" bind:value={route.description} placeholder="Description" readonly={!canEdit}/>
            {#if canEdit}
                <div>
                    <div class="container">
                        <a href="/route/{route.id}?token={route.share_token}">Share</a>
                    </div>
                </div>
            {/if}

        </div>
    </div>
</section>
<section>
    <Map route={route} editable={canEdit}/>
</section>
<section class="section">
    {#if canEdit}
        <div class="columns is-centered">
            <div class="column is-half is-offset-one-quarter">
                <div class="file">
                    <label class="file-label">
                        <input class="file-input" type="file" name="file" id="file" bind:files>
                        <span class="file-cta">
                          <span class="file-label">
                            Upload a GPX file
                          </span>
                        </span>
                        <span class="file-name">
                            {#if files && files.length > 0}
                                {files[0].name}
                            {/if}
                        </span>
                    </label>
                </div>
            </div>
        </div>
        <div class="columns is-centered">
            <div class="column is-one-third has-text-centered">
                <button class="button is-info" on:click={uploadGPX}>Upload</button>
            </div>
            <div class="column is-one-third has-text-centered">
            </div>
            <div class="column is-one-third has-text-centered">
                <button class="button is-primary" on:click={save}>Save</button>
            </div>
        </div>
    {/if}
</section>
