import {PUBLIC_BASE_URL} from '$env/static/public';

export async function load({fetch,  parent }) {
    const {user} = await parent();

    return {};
}