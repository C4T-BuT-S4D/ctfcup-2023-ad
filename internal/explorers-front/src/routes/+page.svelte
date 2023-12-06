<script>
    import RouteList from './RouteList.svelte';
    import world from '$lib/images/world.png';
    import fileUpload from '$lib/images/file-upload.png';
    import edit from '$lib/images/edit.png';
    import share from '$lib/images/share.png';

    /** @type {import('./$types').LayoutData} */
    export let data;

    let user = data.user;
    let error;
</script>
{#if error}
    <p style="color: red">{error}</p>
{/if}
<section class="section">
    <div class="columns is-centered">
        <div class="column is-half has-text-centered">
<!--            <div class="container is-max-desktop">-->
                {#if user}
                    <h2 class="title is-3">Welcome back, {user.username}!</h2>
                {:else}
                    <h3 class="title is-3">Welcome to the "Explorers" service!</h3>
                {/if}
        </div>
    </div>
</section>
{#if user}
    <section class="section">
        <div class="columns is-centered">
            <div class="column is-half has-text-centered">
                <RouteList/>
            </div>
        </div>
        <div class="columns is-centered">
            <div class="column is-half has-text-centered">
                <a class="button is-info" href="/create">Create new route</a>
            </div>
        </div>
    </section>
{:else}
    <section class="section">
        <nav class="level">
            <div class="level-item has-text-centered">
                <div>
                    <p class="heading">Record your expedition route using smart trackers.</p>
                    <p class="title">Record</p>
                    <img width="32px" src="{world}" alt="">
                </div>
            </div>
            <div class="level-item has-text-centered">
                <div>
                    <p class="heading">Upload your route to the service!</p>
                    <p class="title">Upload</p>
                    <img alt="" src="{fileUpload}" width="32px">
                </div>
            </div>
            <div class="level-item has-text-centered">
                <div>
                    <p class="heading">Or create the route manually</p>
                    <p class="title">Create</p>
                    <img width="32px" src="{edit}" alt="">
                </div>
            </div>
            <div class="level-item has-text-centered">
                <div>
                    <p class="heading">Share your route with colleagues and management.</p>
                    <p class="title">Share</p>
                    <img width="32px" src="{share}" alt="">
                </div>
            </div>
        </nav>
    </section>
    <section class="section">
        <div class="columns is-centered">
            <div class="column is-half has-text-centered">
                    <p class="is-size-4">Ready to start?</p>
            </div>
        </div>
       <nav class="level">
            <div class="level-item has-text-centered">
                <div>
                    <a class="button is-medium is-link" href="/signin">Sign In</a>
                </div>
            </div>
            <div class="level-item has-text-centered">
                <div>
                    <a class="button is-medium is-primary" href="/signup">Sign Up</a>
                </div>
            </div>
        </nav>
    </section>
{/if}