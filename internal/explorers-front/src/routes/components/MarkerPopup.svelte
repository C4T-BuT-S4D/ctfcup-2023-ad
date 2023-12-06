<script>
    import {createEventDispatcher} from 'svelte';

    const dispatch = createEventDispatcher();

    export let title;
    export let desc;
    export let comment;
    export let editable;

    function onEdit() {
        dispatch('change', {
            title,
            desc,
            comment
        });
    }

    function onDelete() {
        dispatch('delete', {
            title
        });
    }

</script>

<style>
</style>
<div>
    {#if editable}
        <button class="button is-small is-danger" on:click={onDelete}>Delete</button>
        <div style="width:100%;text-align:center;font-weight:400">
            Title:
            <input class="input is-static" type="text" name="title"
                   on:change={onEdit} bind:value="{title}"
                   style="width: 100%;text-align: center;font-weight:600"/>
            Description:
            <input class="input is-static" type="text" name="desc"
                   on:change={onEdit} bind:value="{desc}"
                   style="width: 100%;text-align: center;font-weight:600"/>
            Comment:
            <input class="input is-static" type="text" name="desc"
                   on:change={onEdit} bind:value="{comment}"
                   style="width: 100%;text-align: center;font-weight:600"/>
        </div>
    {:else }
        <div style="width: 100%;text-align: center;">
            <p><b>{title}</b></p>
            <p>{desc}</p>
            <p>{comment}</p>
        </div>
    {/if}

</div>

