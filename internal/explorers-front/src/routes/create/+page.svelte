<script>
    import {invalidateAll, goto} from '$app/navigation';
    import edit from '$lib/images/edit.png';
    import TextOutline from 'svelte-ionicons/TextOutline.svelte';
    import {PUBLIC_BASE_URL} from "$env/static/public";

    /** @type {any} */
    let error;

    /** @param {{ currentTarget: EventTarget & HTMLFormElement}} event */
    async function handleSubmit(event) {
        const data = new FormData(event.currentTarget);

        try {
            const res = await fetch(`${PUBLIC_BASE_URL}/route/create`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                credentials: 'include',
                body: JSON.stringify({
                    title: data.get('title'),
                    description: data.get('description')
                })
            });
            if (res.ok) {
                await goto('/', {invalidateAll: true});
                return;
            }
            error = await res.text();
            try {
                error = JSON.parse(error).detail.error;
            } catch (err) {
                // ignore
            }
        } catch (err) {
            error = err.toString();
        }
        await invalidateAll();
    }
</script>
<section class="section">
    {#if error}
        <div class="container">
            <div class="notification is-danger">
                {error}
            </div>
        </div>
    {/if}
</section>
<section class="section">
    <div class="columns is-centered">
        <div class="column is-half has-text-centered">
            <img width="64" src="{edit}" alt="">
            <p class="is-size-4">Create a new route</p>
        </div>
    </div>
    <div class="columns is-centered">
        <div class="column is-half has-text-centered">
            <form on:submit|preventDefault={handleSubmit}>
                <div class="field">
                    <p class="control has-icons-left has-icons-right">
                        <input class="input" name="title" type="text" placeholder="Title">
                        <span class="icon is-small is-left">
                            <TextOutline/>
                        </span>
                    </p>
                </div>
                <div class="field">
                    <textarea name="description" class="textarea"></textarea>
                </div>
                <div class="field">
                    <p class="control">
                        <input type="submit" value="Create" class="button is-success">
                    </p>
                </div>
            </form>
        </div>
    </div>
</section>