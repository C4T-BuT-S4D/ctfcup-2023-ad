import {PUBLIC_BASE_URL} from '$env/static/public';

export async function load({fetch,  parent, params, url}) {
    const {user} = await parent();

    let slug = params.slug;
    let shareToken = url.searchParams.get('token');
    let routeURL = `${PUBLIC_BASE_URL}/route/${slug}` + (shareToken ? `?token=${shareToken}` : '');

    let res = await fetch(routeURL, {
        credentials: 'include'
    });

    if (res.ok) {
        let route = await res.json();
        return {route: route, error: null};
    }

    let error = await res.text();
    try {
        error = await res.json().detail.error;
    } catch (e) {
        // ignore.
    }


    return {route: {}, error: error};
}